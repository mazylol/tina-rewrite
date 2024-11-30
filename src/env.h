#ifndef ENV_H
#define ENV_H
#include <concord/discord.h>

struct env_vars {
    const char *dev_token;
    const char *prod_token;
    u64snowflake guild_id;
    bool prod;
};

struct env_vars loadEnvVars();

#endif