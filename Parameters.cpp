#include <argp.h>
#include <cstdlib>

#include "Parameters.h"

const char *argp_program_version = "LatticeMERT v0.1";

static const char *program_doc = "Lattice MERT optimization";

static char args_doc[] = "INPUT";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",  'v', "level",   0, "Produce verbose output" },
//    {"quiet",    'q', 0,      0,  "Don't produce any output" },
//    {"silent",   's', 0,      OPTION_ALIAS },
    {"reference", 'r', "file", 0, "Reference translation file" },
    {"lambda",   'l', "weights", 0, "Feature weights (lambda vector)" },
    { 0 }
};

void Parameters::parseLambdas(const char *str)
{
    string suffix(str);
    size_t pos1 = suffix.find_first_not_of(' ');
    while (pos1 < suffix.size()) {
        size_t pos2 = suffix.find_first_of(' ', pos1);

        // get the token
        const string &token = (pos2 != string::npos) ? suffix.substr(pos1, pos2 - pos1) : suffix.substr(pos1);
        // check if it is not a descriptor
        double lambda = atof(token.c_str());
        lambdas.push_back(lambda);

        if (pos2 == string::npos) break;
        pos1 = suffix.find_first_not_of(' ', pos2);
    }
}

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    Parameters *arguments = (Parameters *)state->input;

    switch (key)
         {
         case 'v':
           arguments->verbose = atoi(arg);
           break;

         case 'r':
           arguments->referencePath = arg;
           break;

         case 'l':
           arguments->parseLambdas(arg);
           break;

         case ARGP_KEY_ARG:
           if (state->arg_num >= 1) argp_usage(state);
           arguments->inputPath = arg;
           break;

         case ARGP_KEY_END:
           //if (state->arg_num < 1)
           //    argp_usage (state);
           if (arguments->referencePath == 0)
                argp_usage(state);
           break;

         default:
           return ARGP_ERR_UNKNOWN;
         }
       return 0;
}

void Parameters::parse(int argc, char **argv)
{
    argp a = { options, parse_opt, args_doc, program_doc };
    argp_parse(&a, argc, argv, 0, 0, this);
}
