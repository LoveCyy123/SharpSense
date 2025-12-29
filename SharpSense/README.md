# SharpSense

> A data-driven assistant that reviews your League of Legends games  
> and gives sharp suggestions on builds, runes and playstyle.

SharpSense is a personal League of Legends companion that helps you **analyze past games** and **plan better strategies** for future ones.  
It focuses on three main game modes:

- **Summoner’s Rift** – Solo/Duo, Flex, Normal (Classic 5v5)
- **Hextech Arena** – “海克斯大乱斗” style fights
- **ARAM** – Howling Abyss all-random games

SharpSense connects to external data sources (such as OP.GG or the Riot API) to inspect your match history and provide **rune pages, item builds and game plan suggestions** tailored to your playstyle and champion pool.

---

## Features (Planned / In Progress)

### 1. Summoner’s Rift Analyzer (Ranked / Flex / Normal)

For Solo/Duo, Flex and Normal games on Summoner’s Rift:

- Aggregate **match history** per champion / role
- Identify common patterns:
  - Early game deaths, CS issues, vision score, damage share, etc.
- Suggest:
  - **Optimal rune setups** for your most played champions
  - **Item build paths** based on matchup and game length
  - **Playstyle tips** (lane management, teamfight timing, power spike usage)

### 2. Hextech Arena Advisor

For **Hextech Arena / 海克斯大乱斗** style modes:

- Recommend **augments / hexes** (when data is available) based on:
  - Champion, class, and synergy with your previous choices
- Suggest **item builds** that fit:
  - Round length
  - Enemy composition
  - Your champion’s strengths (burst, sustain, front line, etc.)

### 3. ARAM Coach

For **ARAM (All Random All Mid)** games:

- Analyze your past ARAM games by champion
- Recommend:
  - **Starting items** and core builds
  - **Rune pages** tuned for poke / all-in / sustain comps
  - **Playstyle advice** based on team composition:
    - Poke comp, hard engage comp, protect-the-carry comp, etc.

---

## How It Works (Concept)

> ⚠️ Work in progress – actual implementation may change.

1. **Data Collection**
   - Pull match history from external services (e.g. OP.GG or the Riot API), or import JSON exports.
   - Parse champions, runes, items, KDA, CS, damage, game length, etc.

2. **Analysis Engine**
   - Aggregate stats across games for:
     - Each champion
     - Each role / lane
     - Each game mode (Ranked, Hextech, ARAM)
   - Compare your builds and runes against high-winrate templates.
   - Detect repeated issues (e.g. dying before first core item, low vision, over-aggressive laning).

3. **Recommendation Layer**
   - Output:
     - Suggested rune pages
     - Suggested item build orders
     - Short, concrete tips (“Play around level 2 spike”, “Rush anti-heal in this matchup”, etc.)

4. **User Interface**
   - **Planned GUI**: cross-platform desktop app built with **Qt / PyQt**.
   - Optionally: command-line utilities for quick analysis and debugging.

---

## Tech Stack (Planned)

- **Language:** Python (core logic), with potential C++/Qt modules later
- **UI:** Qt / PyQt (desktop GUI)
- **Data:** Riot API / OP.GG / local match data (depending on what you configure)
- **Platform:** macOS and Windows (Linux support later if needed)

---

## Project Status

> 🔨 This project is under active development.

Planned milestones:

- [ ] Basic data ingestion from OP.GG / Riot API  
- [ ] Core analysis for Summoner’s Rift matches  
- [ ] ARAM build & rune suggestion engine  
- [ ] Hextech Arena advisor (augments + items)  
- [ ] Qt / PyQt desktop UI prototype  
- [ ] Exportable reports (Markdown / HTML / JSON)

---

## Getting Started (Development)

> This section assumes a Python environment. Adjust as needed once the project structure is finalized.

```bash
# Clone the repository
git clone https://github.com/<your-username>/SharpSense.git
cd SharpSense

# Create and activate a virtual environment (optional but recommended)
python -m venv .venv
source .venv/bin/activate    # on macOS / Linux
# .venv\Scripts\activate     # on Windows

# Install dependencies
pip install -r requirements.txt
