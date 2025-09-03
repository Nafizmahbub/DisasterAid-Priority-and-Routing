# DisasterAid: Beneficiary Prioritization & Shortest-Path Routing

Prioritize flood-affected beneficiaries and compute shortest paths between cities using **Dijkstra**, **Bellman–Ford**, and **Floyd–Warshall**.

## Features
- Custom priority rules:
  - Children (≤ 12) first, then Elders (≥ 60), then Adults.
  - Within same group: Females first.
  - Tiebreaker: For children, younger first; for others, older first; finally by name (A→Z).
- Stable, in-place **Merge Sort** for beneficiary ordering.
- Graph setup with named cities and non-negative road distances.
- Shortest-path results for a chosen source→destination and routes to each beneficiary's city.
- Clean console tables.

## Build & Run (CMake)
```bash
# 1) Configure & build
cmake -S . -B build
cmake --build build --config Release

# 2) Run
./build/disaster_aid
```

## Build & Run (g++)
```bash
g++ -std=gnu++17 -O2 -Wall -Wextra -o disaster_aid src/main.cpp
./disaster_aid
```

## Sample Input
You can paste from `sample_data/example_input.txt` during runtime. It contains:
- 3 beneficiaries
- 4 cities, 5 roads
- Example source/destination

## Bangla (বাংলা নির্দেশনা)
- কোডটি **C++17** দিয়ে লেখা।
- প্রথমে বন্যায় ক্ষতিগ্রস্ত লোকজনের তথ্য ইনপুট নিবে (Name, Age, Gender, City) এবং নিয়ম মেনে **Merge Sort** দিয়ে সাজাবে।
- এরপর শহর ও রাস্তার গ্রাফ নিবে (সকল distance ≥ 0)।
- তারপর আপনার দেয়া source → destination এর জন্য **Dijkstra**, **Bellman–Ford**, **Floyd–Warshall** এর ফলাফল দেখাবে।
- শেষে প্রতিটি beneficiary এর শহরে যেতে কত দূরত্ব এবং কী path তা দেখাবে।

## Priority Rules Recap
1. Children (≤ 12) → Elders (≥ 60) → Adults.
2. Within group: `Female` → `Male`.
3. If still tied:
   - Children: lower age first.
   - Elders/Adults: higher age first.
   - Then lexicographic by name (A→Z).

## License
MIT License — see `LICENSE`.
