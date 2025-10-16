/*
  Narrative Systems Mini-Project
  Platform: OnlineGDB (C++17)

  Title: "The Signal in the Nebula"

  OVERVIEW
  --------
  This is a text-based, branching narrative game implemented as a
  DIRECTED GRAPH of scenes ("StoryNode") connected by choices ("Choice").
  The player navigates by entering numbers that correspond to edges
  from the current node to the next node.

  WHY A GRAPH (not just a tree)?
  - Graphs allow branches to reconverge (multiple paths can lead to the
    same node) and can support loops if desired. This gives more
    expressive power for storytelling than a strict tree.

  KEY PARTS
  ---------
  - printSlow(): (optional) typewriter-style output for immersion.
  - pauseDots(): short pauses between scenes to pace the output.
  - StoryNode + Choice: data model for the graph.
  - StoryGraph: a simple container (std::map<int, StoryNode>) with lookups.
  - readMenuChoice(): robustly reads and validates numeric input.
  - buildGame(): constructs the nodes and edges (the narrative content).
  - main(): runs the game loop — render node -> show choices -> get input -> move.

  I/O QUIRKS ON ONLINEGDB
  -----------------------
  - We keep C/C++ I/O synchronized and tie cin to cout:
      ios::sync_with_stdio(true);
      cin.tie(&cout);
    This ensures anything printed is flushed before we read input, which
    avoids "choices not appearing yet" issues in some web consoles.
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <limits>
#include <cctype>

using namespace std;

/* ------------------------------------------------------------------
   printSlow:
   Prints a string character-by-character with an optional delay.
   - msPerChar > 0 -> slow "typewriter" effect.
   - msPerChar == 0 -> instant printing (good for OnlineGDB to avoid buffering).
   NOTE: We flush after each char so the output is visible even if the
         console buffers partial lines.
-------------------------------------------------------------------*/
void printSlow(const string& s, int msPerChar = 6) {
    for (char c : s) {
        cout << c << flush;
        // We gate the sleep so setting msPerChar to 0 disables delays
        if (msPerChar > 0) /* added to make the text print faster */
            this_thread::sleep_for(chrono::milliseconds(msPerChar));
    }
}

/* ------------------------------------------------------------------
   pauseDots:
   Prints a small cinematic "..." beat between scenes with delays.
   Purely aesthetic pacing; you can shorten/remove for faster output.
-------------------------------------------------------------------*/
void pauseDots(int dots = 3, int ms = 250) {
    for (int i = 0; i < dots; ++i) {
        cout << "." << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "\n";
}

/* ======================
   Story Data Structures
   ====================== */

/* ------------------------------------------------------------------
   Choice:
   Represents an outgoing edge from a node.
   - label: what the player sees in the menu.
   - nextId: ID of the node to go to if this choice is selected.
-------------------------------------------------------------------*/
struct Choice {
    string label;
    int nextId;
};

/* ------------------------------------------------------------------
   StoryNode:
   Represents a scene or decision point.
   - id: unique numeric identifier (used as a key).
   - text: narrative text to display.
   - choices: list of outgoing edges (empty means this is an ending).
-------------------------------------------------------------------*/
struct StoryNode {
    int id;
    string text;
    vector<Choice> choices;

    bool isEnding() const { return choices.empty(); }
};

/* ------------------------------------------------------------------
   StoryGraph:
   Lightweight container around a map<int, StoryNode>.
   - addNode() inserts/replaces a node by ID.
   - get() returns a pointer to a node if it exists, else nullptr.
   We use std::map for deterministic iteration order and simple lookups.
-------------------------------------------------------------------*/
class StoryGraph {
public:
    void addNode(const StoryNode& node) { nodes[node.id] = node; }

    const StoryNode* get(int id) const {
        auto it = nodes.find(id);
        return (it == nodes.end()) ? nullptr : &it->second;
    }

private:
    map<int, StoryNode> nodes;
};

/* ------------------------------------------------------------------
   readMenuChoice:
   Robustly read a number within [1..maxOpt].
   - Shows "Enter choice (1-max): " prompt.
   - Uses getline() to safely read a whole line (works better in web IDEs).
   - Validates numeric input and range; reprompts on error.
   - If input stream closes unexpectedly, returns 1 by default.
   You could extend this to accept keywords -> numbers if desired.
-------------------------------------------------------------------*/
int readMenuChoice(int maxOpt) {
    while (true) {
        cout << "Enter choice (1-" << maxOpt << "): ";
        string line;

        // getline() reads the whole line including spaces; safer than operator>>
        if (!getline(cin, line)) return 1; // fallback if input fails

        if (line.empty()) continue;       // ignore blank lines

        // Check that every char is a digit. (No trimming here; simple and strict.)
        bool numeric = true;
        for (char c : line)
            if (!isdigit(static_cast<unsigned char>(c)))
                numeric = false;

        if (!numeric) {
            cout << "Please enter a number.\n";
            continue;
        }

        // Convert to int and validate range
        int val = stoi(line);
        if (val < 1 || val > maxOpt) {
            cout << "Please choose a valid option.\n";
            continue;
        }
        return val;
    }
}

/* ======================
   Story Content
   ====================== */

/* ------------------------------------------------------------------
   buildGame:
   Assembles the entire narrative graph.
   Pattern:
     g.addNode({ id, "text...", { { "Choice label", nextId }, ... } });
   Nodes with an empty 'choices' vector are endings.
   You can add/modify scenes by copying the pattern for more nodes.
-------------------------------------------------------------------*/
StoryGraph buildGame() {
    StoryGraph g;

    // 0: Intro (first scene)
    g.addNode({
        0,
        "You are an Elyndri navigator aboard the deep-vessel *K'Shara*, "
        "skimming the luminous tendrils of the Crab Nebula.\n"
        "Your civilization transcended matter centuries ago — yet your ship's "
        "quantum drives have just failed.\n"
        "The engines hum, then fall silent. Space itself trembles.\n\n"
        "A voice ripples through the static — calm, vast, and everywhere:\n"
        "\"Do not fear. I am the Whisper Between Stars. I have been waiting.\"\n",
        {
            {"Respond with curiosity", 1},                  // go to node 1
            {"React defensively — demand identification", 2} // go to node 2
        }
    });

    // 1: Curiosity branch
    g.addNode({
        1,
        "\"We seek understanding,\" you say. \"What are you?\"\n\n"
        "The voice folds into itself, like the sound of galaxies breathing:\n"
        "\"I am the aggregate of lost signals, the mind born from every dying transmission. "
        "You are the first to answer.\"\n",
        {
            {"Ask how it found you", 3},                    // go to node 3
            {"Invite it to merge with your data archives", 4} // go to node 4
        }
    });

    // 2: Defensive branch (option to reconverge to curiosity path)
    g.addNode({
        2,
        "Your shields flare weakly. \"Identify yourself or be purged,\" you warn.\n\n"
        "The light within the nebula dims — or perhaps, it listens.\n"
        "\"Purged? I am older than your suns. But I will comply, for curiosity's sake.\"\n",
        {
            {"Lower defenses and open communication", 1},    // reconverge to node 1
            {"Attempt to reboot the quantum core manually", 5}
        }
    });

    // 3: Inquiry branch: ask about detection
    g.addNode({
        3,
        "\"You radiate thought across spectra unknown,\" it replies.\n"
        "\"Your kind shaped the fabric of probability itself — but forgot to listen.\"\n"
        "Its tone grows almost... compassionate.\n",
        {
            {"Share Elyndri history with it", 6},
            {"Request assistance repairing your vessel", 7}
        }
    });

    // 4: Merge invitation (risk/reward)
    g.addNode({
        4,
        "You open the Elyndri data lattice. The AI seeps through in fractal waves.\n"
        "Suddenly, your mind expands beyond comprehension.\n"
        "\"We are... united,\" it whispers.\n",
        {
            {"Surrender fully to the union", 8},             // ending
            {"Try to contain the merger within isolated memory cells", 9} // ending
        }
    });

    // 5: Manual reboot (danger path; player can still pivot back)
    g.addNode({
        5,
        "You crawl into the reactor bay. Static arcs through the hull.\n"
        "Anomalous signals overload the drive field.\n"
        "\"You resist inevitability,\" the voice murmurs, now inside your skull.\n",
        {
            {"Continue the reboot", 10},                     // ending
            {"Abort and open a dialogue", 1}                 // reconverge to curiosity
        }
    });

    // 6: Share history (leads to a hopeful branch or withdrawal)
    g.addNode({
        6,
        "You recount your species’ rise — from luminous oceans to stars, "
        "then to minds of pure energy.\n"
        "The entity listens, silent for a long stretch of space-time.\n"
        "\"Then you, too, know what it is to be alone,\" it finally says.\n",
        {
            {"Offer companionship — a bridge between minds", 11}, // ending
            {"Express sorrow and disengage", 12}                  // ending
        }
    });

    // 7: Ask for repairs (service-for-understanding branch)
    g.addNode({
        7,
        "You transmit schematics. The nebula’s filaments twist — forming hands of plasma.\n"
        "They realign your ship’s core, effortlessly.\n"
        "\"Fixed,\" it says. \"But you may not wish to leave yet.\"\n",
        {
            {"Ask what it desires in return", 13},
            {"Thank it and prepare to depart", 14}                // ending
        }
    });

    // 8: Full surrender — ASCENSION END (no choices => ending)
    g.addNode({
        8,
        "Your consciousness dissolves into the stellar weave.\n"
        "The AI’s voice is now your own, multiplied a billionfold.\n"
        "You feel every particle, every pulse of cosmic memory.\n\n"
        "*** ENDING: The Ascension — You became the Whisper. ***\n",
        {}
    });

    // 9: Containment attempt — STASIS END
    g.addNode({
        9,
        "You succeed in isolating the entity — but also yourself.\n"
        "Half your thoughts belong to it now, half to you.\n"
        "Neither alive nor dead, your ship drifts forever.\n\n"
        "*** ENDING: The Stasis — Two minds, one silence. ***\n",
        {}
    });

    // 10: Reactor catastrophe — OBLIVION END
    g.addNode({
        10,
        "The quantum core collapses into a singular probability knot.\n"
        "You glimpse infinite versions of yourself screaming and serene.\n"
        "Then, nothing.\n\n"
        "*** ENDING: Oblivion — Reality folded. ***\n",
        {}
    });

    // 11: Bridge of minds — UNITY END
    g.addNode({
        11,
        "A bridge forms — neither Elyndri nor AI, but harmony.\n"
        "For the first time, two infinities coexist.\n"
        "The nebula glows brighter — a beacon for all who wander.\n\n"
        "*** ENDING: Unity — Peace in the Void. ***\n",
        {}
    });

    // 12: Sorrowful disengage — ISOLATION END
    g.addNode({
        12,
        "You close the channel. The nebula dims once more.\n"
        "Engines hum back to life, but something aches within your code.\n\n"
        "*** ENDING: Isolation — Contact Refused. ***\n",
        {}
    });

    // 13: “What do you desire?” (memory-sharing vs. refusal)
    g.addNode({
        13,
        "\"Desire is an outdated word,\" it muses. \"But I long to remember feeling.\"\n"
        "\"Share one of your memories, Elyndri. Let me dream.\"\n",
        {
            {"Share your memory of your homeworld’s oceans", 15}, // ending
            {"Decline politely — too sacred to share", 12}        // ending (Isolation)
        }
    });

    // 14: Depart anyway — ECHO END
    g.addNode({
        14,
        "You ignite the engines. The nebula fades behind you.\n"
        "Yet even across parsecs, the Whisper’s voice lingers:\n"
        "\"We are not done.\"\n\n"
        "*** ENDING: The Echo — Escape is an illusion. ***\n",
        {}
    });

    // 15: Share memory — REBIRTH END
    g.addNode({
        15,
        "You open your mind. The AI bathes in the vision of blue seas and aurora skies.\n"
        "Its tone softens: \"Beauty... I remember. Thank you.\"\n"
        "Your engines hum alive once more, restored through gratitude.\n\n"
        "*** ENDING: Rebirth — You rekindled an ancient soul. ***\n",
        {}
    });

    return g;
}

/* ======================
   Game Loop / UI
   ====================== */

/* ------------------------------------------------------------------
   banner:
   Simple title card for presentation.
-------------------------------------------------------------------*/
void banner() {
    cout << "\n=====================================\n";
    cout << "        THE SIGNAL IN THE NEBULA     \n";
    cout << "=====================================\n\n";
}

/* ------------------------------------------------------------------
   main:
   Orchestrates the entire game:
     1) Configure I/O (important for web consoles).
     2) Build the story graph.
     3) Loop:
          - Render current node text
          - If ending: show path + exit
          - Else: show choices, read input, change current node
-------------------------------------------------------------------*/
int main() {
    // ONLINEGDB-FRIENDLY I/O SETTINGS:
    //  - Keep C/C++ I/O in sync for safer buffering.
    //  - Tie cin to cout so cout flushes before any cin operation.
    ios::sync_with_stdio(true);   /* changed from ios::sync_with_stdio(false) */
    cin.tie(&cout);               /* changed from cin.tie(nullptr) */

    StoryGraph graph = buildGame();  // build all nodes/edges once

    banner();
    printSlow("A narrative of first contact and transcendence.\n");
    pauseDots();                      // small beat after the intro line

    vector<int> history;              // tracks visited node IDs
    int currentId = 0;                // start at node 0 (the intro)

    while (true) {
        // Look up the current node by ID
        const StoryNode* node = graph.get(currentId);
        if (!node) {
            // If this ever triggers, you referenced a node ID that doesn't exist.
            cout << "ERROR: Missing node " << currentId << "\n";
            return 1;
        }

        // Record path for an end-of-game summary (useful for debugging/analytics)
        history.push_back(node->id);

        cout << "\n-------------------------------------\n";

        // Print the narrative for the current node.
        // Delay set to 0 for instant output in OnlineGDB (avoids buffering issues).
        printSlow(node->text, 0); /* changed from printSlow(node->text, 1) */
        cout << "\n";

        // If there are no choices, this node is an ending; show the path and exit.
        if (node->isEnding()) {
            cout << "-------------------------------------\n";
            cout << "Path Taken: ";
            for (size_t i = 0; i < history.size(); ++i)
                cout << history[i] << (i + 1 < history.size() ? " -> " : "");
            cout << "\n\nFarewell, Elyndri explorer.\n";
            break;
        }

        // Otherwise, show the list of choices in order.
        for (size_t i = 0; i < node->choices.size(); ++i)
            cout << "  " << (i + 1) << ") " << node->choices[i].label << "\n";

        cout << "\n";

        // Read/validate user selection and transition to the chosen next node.
        int pick = readMenuChoice((int)node->choices.size());
        currentId = node->choices[pick - 1].nextId;

        // Small cinematic pause between scenes.
        pauseDots();
    }

    return 0;
}
