#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <concord/discord.h>
#include <dotenv.h>

#include "intents.h"

struct env_vars {
    const char *dev_token;
    const char *prod_token;
    u64snowflake guild_id;
    bool prod;
};

struct env_vars loadEnvVars() {
    struct env_vars env_vars = {
        .prod = false, .guild_id = 0, .prod_token = "", .dev_token = ""};

    env_load(".", false);

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

struct env_vars vars = {};

int main() {
    vars = loadEnvVars();

    size_t num_intents;
    struct RawIntents *rawIntents = loadRawIntents("intents.json", &num_intents);

    struct Intents *intents = init();

    for (int i = 0; i < num_intents; i++) {
        addIntent(intents, rawIntents[i].pattern, rawIntents[i].responses);
    }

    for (int i = 0; i< num_intents; i++) {
        if (strcmp(intents->intents[i]->pattern, "test") == 0) {
            addResponse(intents, "test", "test response");
        }
    }

    generateJson(intents, "intents.json");

    freeRawIntents(rawIntents, num_intents);
    freeIntents(intents);

    return 0;
}
