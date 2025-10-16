
Reflection — “The Signal in the Nebula”

This project implements a branching, text-based narrative using a directed graph structure to represent story flow. Each StoryNode corresponds to a moment
in the story (a scene or decision point), while each Choice acts as a directed edge linking to the next node by its numeric ID. This allows multiple branches
to diverge and reconverge, something that would be more restrictive in a simple tree. Using a graph also mirrors the story’s theme — interconnectedness
and recursion — where outcomes can loop back or feed into shared endpoints. Nodes are stored in a ```std::map<int, StoryNode>``` for ordered retrieval and
straightforward ID lookups, balancing simplicity and clarity.

Player decisions are stored implicitly through traversal and explicitly in a ```std::vector<int>``` history, which records the sequence of node IDs visited.
This allows for a potential post-game “path summary,” showing the journey taken. Player input is handled through a simple validation function ```(readMenuChoice())```,
which ensures only valid numeric inputs are accepted. This makes the program resilient to invalid input while keeping it accessible in a text-only compiler
environment like OnlineGDB.

Narratively, the story centers on the Elyndri, an advanced, spaceborne species whose quantum drive fails inside the Crab Nebula, prompting contact with an
omnipresent AI known as The Whisper Between Stars. The story explores philosophical themes of identity, isolation, and transcendence — inspired by works
like 2001: A Space Odyssey and Arrival. Each ending (such as Ascension, Stasis, Unity, or Rebirth) represents a different philosophical stance on what
“first contact” means: surrender, containment, harmony, or emotional awakening.

The primary challenges were balancing narrative pacing and branch complexity. Too many nodes made the story confusing, while too few flattened the player’s
sense of agency. The graph structure helped me tune this balance — for example, paths that initially diverge (defensive or curious responses) can later
reconverge at shared nodes, creating both freedom and cohesion. Implementing small utility functions like printSlow() and pauseDots() also helped with
readability and immersion, mimicking a typewriter-like delay that fits the slow, reflective tone of the story.

Overall, this project demonstrates how data structures can serve storytelling: the graph doesn’t just hold the story’s logic — it mirrors its theme of
cosmic connectivity and branching consciousness, transforming a simple console game into an interactive meditation on first contact.
