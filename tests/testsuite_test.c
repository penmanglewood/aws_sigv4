#include "minunit.h"
#include "aws_status.h"
#include "aws_sigv4.h"
#include <stdio.h>
#include <dirent.h>

#define LINE_LEN 256

#define SUITE_PATH "./tests/aws4_testsuite/"

struct file {
    char authz_name[255];
    char req_name[255];
    FILE *authz_file;
    FILE *req_file;
};

struct pair {
    char *key;
    char *val;
};

void cleanup_files(struct file *file);
int prepare_files(char *raw_filename, struct file *file);

char *test_aws_testsuite()
{
    DIR *dp;
    struct dirent *ep;

    struct file file;

    char line[LINE_LEN];

    char *date = "Mon, 09 Sep 2011 23:36:00 GMT";
    char *secret = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY";
    char calculated_signature[65];
    char correct_signature[65];
    char *path;
    char pathonly[255];
    char *question_mark_pos;
    char *method;
    char *param_str = NULL;
    char *kv_pair = NULL;
    char *kv_str;
    char *kvtok_str;
    char pair_tok[255];
    struct pair kv;

    char out[1024];

    aws_t context;

    dp = opendir(SUITE_PATH);
    if (dp != NULL)
    {
        while ((ep = readdir(dp))) {
            if (ep->d_name[0] != '.') {

                if (strstr(ep->d_name, ".authz") == NULL)
                    continue;

                if (prepare_files(ep->d_name, &file) == AWS_ERR)
                    continue;

                /* Set signature */
                fseek(file.authz_file, -64, SEEK_END);
                size_t len = fread(correct_signature, 64, 1, file.authz_file);
                correct_signature[64] = '\0';

                /* Get the first line from the request file */
                fgets(line, LINE_LEN, file.req_file);
                if (ferror(file.req_file)) {
                    printf("Some problem with %s\n", file.req_name);
                    fclose(file.req_file);
                    clearerr(file.req_file);
                    continue;
                }

                method = strtok(line, " ");
                path = strtok(NULL, "\n");
                path[strlen(path) - 10] = '\0';

                // Cut path at the "?" if it has params
                strcpy(pathonly, path);
                question_mark_pos = strstr(path, "?");
                if (question_mark_pos != NULL)
                    pathonly[question_mark_pos-path] = '\0';

                // DEBUG only test GET for now
                if (strcmp(method, "GET") != 0) {
                    cleanup_files(&file);
                    continue;
                }

                context = aws_init("us-east-1", "host", "host.foo.com", pathonly, method);

                param_str = strstr(path, "?");
                if (param_str != NULL) {
                    param_str++; /* Skip "?" */
                    for (kv_pair = strtok_r(param_str, "&", &kv_str); kv_pair; kv_pair = strtok_r(NULL, "&", &kv_str)) {
                        strcpy(pair_tok, kv_pair);
                        kv.key = strtok_r(pair_tok, "=", &kvtok_str);
                        kv.val = strtok_r(NULL, "\n", &kvtok_str);

                        aws_add_param(context, kv.key, kv.val);
                    }
                }

                aws_sign(context, secret, date, calculated_signature);

                // DEBUG
                /* aws_debug(context, out); */
                /* printf("%s %s\n%s vs\n%s\n\n", path, file.req_name, calculated_signature, correct_signature); */
                /* printf("-->\n%s\n<--\n", out); */

                mu_assert(strcmp(calculated_signature, correct_signature) == 0, "Signatures don't match");

                cleanup_files(&file);
                aws_cleanup(context);
            }
        }

        (void)closedir(dp);
    }
    else
        perror("couldn't open dir");


    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_aws_testsuite);

    return NULL;
}

RUN_TESTS(all_tests);

/** UTILITIES **/

/**
 * Get filenames and open files
 * @param raw_filename Filename with extension
 * @param file Pointer to struct file where data will be written to
 * @return int 0 for success. 1 for failure.
 */
int prepare_files(char *raw_filename, struct file *file)
{
    char *chopper;

    chopper = strstr(raw_filename, ".");
    raw_filename[chopper - raw_filename] = '\0';

    strcpy(file->req_name, SUITE_PATH);
    strcat(file->req_name, raw_filename);
    strcat(file->req_name, ".req");

    strcpy(file->authz_name, SUITE_PATH);
    strcat(file->authz_name, raw_filename);
    strcat(file->authz_name, ".authz");

    file->req_file = fopen(file->req_name, "r");
    if (!file->req_file)
        return AWS_ERR;

    file->authz_file = fopen(file->authz_name, "r");
    if (!file->authz_file)
        return AWS_ERR;

    return AWS_OK;
}

void cleanup_files(struct file *file)
{
    if (file->req_file)
        fclose(file->req_file);

    if (file->authz_file)
        fclose(file->authz_file);

    file->req_file = NULL;
    file->authz_file = NULL;
}
