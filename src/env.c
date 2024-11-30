#include "env.h"

#include <dotenv.h>
#include <stdlib.h>
#include <string.h>

struct env_vars loadEnvVars() {
    struct env_vars env_vars = {
        .prod = false, .guild_id = 0, .prod_token = "", .dev_token = ""};

    env_load("../", false);

    char *id = getenv("GUILD_ID");
    sscanf(id, "%" SCNu64, &env_vars.guild_id);

    env_vars.dev_token = getenv("DEV_TOKEN");
    env_vars.prod_token = getenv("PROD_TOKEN");

    char *prodValue = getenv("PROD");

    if (prodValue == NULL) {
        env_vars.prod = false;
        return env_vars;
    };

    if (strcmp(prodValue, "0") == 0) {
        env_vars.prod = false;
    } else if (strcmp(prodValue, "1") == 0) {
        env_vars.prod = true;
    }

    return env_vars;
}
