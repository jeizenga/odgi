#include "subcommand.hpp"
#include "odgi.hpp"
//#include "gfakluge.hpp"
#include "args.hxx"
//#include "io_helper.hpp"

namespace odgi {

using namespace odgi::subcommand;

int main_view(int argc, char** argv) {

    // trick argumentparser to do the right thing with the subcommand
    for (uint64_t i = 1; i < argc-1; ++i) {
        argv[i] = argv[i+1];
    }
    std::string prog_name = "odgi view";
    argv[0] = (char*)prog_name.c_str();
    --argc;
    
    args::ArgumentParser parser("projection of graphs into other formats");
    args::HelpFlag help(parser, "help", "display this help summary", {'h', "help"});
    args::ValueFlag<std::string> dg_in_file(parser, "FILE", "load the index from this file", {'i', "idx"});
    args::Flag to_gfa(parser, "to_gfa", "write the graph to stdout in GFA format", {'g', "to-gfa"});
    args::Flag display(parser, "display", "show internal structures", {'d', "display"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    if (argc==1) {
        std::cout << parser;
        return 1;
    }

    graph_t graph;
    assert(argc > 0);
    std::string infile = args::get(dg_in_file);
    if (infile.size()) {
        if (infile == "-") {
            graph.load(std::cin);
        } else {
            ifstream f(infile.c_str());
            graph.load(f);
            f.close();
        }
    }
    if (args::get(display)) {
        graph.display();
    }
    if (args::get(to_gfa)) {
        graph.to_gfa(std::cout);
    }
    
    int path_counter = 0;
    auto path_start = std::chrono::system_clock::now();
    graph.for_each_path_handle([&](const path_handle_t& path_handle){
        cerr << "got path handle " << as_integer(path_handle) << endl;
        for (handle_t handle : graph.scan_path(path_handle)) {
            cerr << "\tat handle " << graph.get_id(handle) << (graph.get_is_reverse(handle) ? "-" : "+") << endl;
            path_counter++;
        }
    });
    cerr << path_counter << endl;

    return 0;
}

static Subcommand odgi_view("view", "projection of graphs into other formats",
                             PIPELINE, 3, main_view);


}
