#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <concord/discord.h>

#include "env.h"
#include "intents.h"

struct env_vars vars = {};
struct Intents *intents = {0};

u64snowflake botId;

void on_ready(struct discord *client, const struct discord_ready *event) {
    botId = event->user->id;
}

void on_message(struct discord *client, const struct discord_message *event) {
    if (event->author->bot) {
        return;
    }

    bool botMentioned = false;

    for (int i = 0; i < event->mentions->size; i++) {
        if (event->mentions->array[i].id == botId) {
            botMentioned = true;
            break;
        }
    }

    if (botMentioned) {
        char *message = event->content;

        message += 23;

        for (int i = 0; i < intents->size; i++) {
            if (strcmp(message, intents->intents[i]->pattern) == 0) {
                unsigned int seed = time(0);

                int responseLen = -1;
                while (intents->intents[i]->responses[++responseLen] != NULL) {
                }

                int idx = rand_r(&seed) % responseLen;

                struct discord_create_message params = {
                    .content = intents->intents[i]->responses[idx]};

                discord_create_message(client, event->channel_id, &params, NULL);

                return;
            }
        }

        struct discord_create_message params = {
            .content = "I'm sorry, I don't understand."};

        discord_create_message(client, event->channel_id, &params, NULL);
    } else {
        u64snowflake msg_id = 0;

        sscanf(event->content, "%" SCNu64, &msg_id);

        if (!msg_id) {
            if (!event->referenced_message)
                return;

            msg_id = event->referenced_message->id;
        }

        for (int i = 0; i < intents->size; i++) {
            if (strcmp(event->referenced_message->content, intents->intents[i]->pattern) == 0) {
                addResponse(intents, event->referenced_message->content, event->content);
                generateJson(intents, "intents.json");

                return;
            }
        }

        addIntent(intents, event->referenced_message->content, (char *[]){event->content, NULL});
        generateJson(intents, "intents.json");
    }
}

int main() {
    vars = loadEnvVars();

    size_t num_intents;
    struct RawIntents *rawIntents = loadRawIntents("intents.json", &num_intents);

    intents = init();

    for (int i = 0; i < num_intents; i++) {
        addIntent(intents, rawIntents[i].pattern, rawIntents[i].responses);
    }

    struct discord *client = {0};

    if (vars.prod) {
        client = discord_init(vars.prod_token);
    } else {
        client = discord_init(vars.dev_token);
    }

    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);

    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);

    discord_run(client);

    freeRawIntents(rawIntents, num_intents);
    freeIntents(intents);

    return 0;
}
