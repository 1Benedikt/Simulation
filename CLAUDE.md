# CLAUDE.md

## What this is
University praktikum project ("Simulation and Analysis of Internet Protocols", Uni Hamburg,
summer term 2025/26) — Topic 1: **Centralized Control for Datacenter Networks**. An
SDN-inspired (but not OpenFlow) simulation built on **OMNeT++ 6.1** and **INET 4.5.4**.
Assignment PDF: `t1_sdn_project.pdf` (in Downloads, not part of the repo).

The actual OMNeT++ project root is the nested `sdnproject/` directory (it has
`.project`/`.cproject`/`package.ned`). The top-level folder is just the extracted zip;
`__MACOSX/`, the stray `.DS_Store` files, and the duplicate top-level `.project` are
macOS zip artifacts, not part of the project — safe to delete. Not currently a git repo.

## Assignment goal
Central SDN workflow: **Observe → Decide → Act**. Pick exactly **one** controller
strategy and implement it end to end:
- **Load Balancing** — spread traffic across paths/server replicas
- **QoS-Aware Control** — differentiate traffic classes, prioritize/allocate resources
- **QoS or Failure Recovery** — detect a link/node/path failure, activate a fallback policy

Requirements: at least one dynamic/adaptive reaction, comparison against the static
baseline, evaluation across multiple scenarios, student-defined research questions,
and a report + README + analysis scripts + oral defense. No strategy has been chosen yet.

## Architecture — control loop
`NetworkMonitor` → `CentralizedTrafficController` → `PolicyEnforcer`, riding on top of an
INET fat-tree-ish datacenter topology.

- **`sdnproject/src/monitor/NetworkMonitor.{h,cc,ned}`** — 10 instances (one per
  core/aggregation/edge router). Periodically sends an abstract `NetworkStateReport`
  (utilization, queueLength, packetLossRate, failed) to the controller.
- **`sdnproject/src/controller/CentralizedTrafficController.{h,cc,ned}`** — central
  logic. Maintains `links` (per-link state) and `topology` (graph), computes BFS
  shortest paths as the baseline, and is the extension point for the chosen strategy.
  **The topology is hardcoded in `buildTopologyGraph()`** — if the `.ned` topology
  changes, this must be updated by hand to match.
- **`sdnproject/src/enforcer/PolicyEnforcer.{h,cc,ned}`** — receives `ControlDecision`
  messages; currently just logs them and stores them in a map (no real actuation yet).
- **`sdnproject/src/messages/ControlMessages.msg`** — defines `NetworkStateReport` and
  `ControlDecision`; generated `_m.h`/`_m.cc` are checked in.
- **`sdnproject/simulations/DatacenterCentralizedControl.ned`** — topology: 2 core +
  4 aggregation + 4 edge routers, 16 hosts, 1 controller, 1 enforcer, 10 monitors.
- **`sdnproject/simulations/omnetpp.ini`** — configs `BaselineStatic` (strategy=baseline),
  `Dynamic` (extends baseline, strategy=dynamic, still a TODO), `LinkFailure` (extends
  Dynamic; sets `monitor[2].failed = true` — this is a simulated signal only, it does
  **not** disable a real INET link/channel).
- **`sdnproject/simulations/demands.csv`** — controller's known traffic matrix; must stay
  consistent with the actual traffic apps configured in `omnetpp.ini`. (Note: the
  assignment PDF refers to this as `data/demands.csv`; in this skeleton it actually
  lives at `simulations/demands.csv`, matching the `demandFile` ini parameter.)

## Current implementation state (skeleton only — baseline works, dynamic strategy is stubbed)
- `CentralizedTrafficController::collectNetworkState()` — empty, TODO.
- `CentralizedTrafficController::runDynamicDecision()` — empty, TODO. This is where the
  chosen strategy's decision logic belongs.
- `CentralizedTrafficController::chooseServer()` — always returns the first candidate,
  TODO (relevant if Load Balancing is chosen).
- `processReport()` has only a minimal threshold-based reroute trigger that acts on a
  single demand (`demands.front()`) — a placeholder, not a real multi-flow strategy.
- `PolicyEnforcer` only logs/records decisions; it doesn't change simulated traffic
  behavior. Students must design the actuation mechanism (e.g., reroute stats, change
  host app parameters/priority at runtime via signals, etc.) and document it.
- `sdn.sh run-all` references `LowLoad`, `HighLoad`, `HotspotTraffic` configs that are
  **not yet defined** in `omnetpp.ini` — needed for the "evaluate under multiple
  scenarios" requirement.

## Build & run
Requires OMNeT++ 6.x + INET 4.5.x. Both are installed locally:
- OMNeT++ **6.3.0** at `/Users/bent/Desktop/omnett/omnetpp-6.3.0` (its `bin/` is on `PATH`,
  so `opp_makemake`/`opp_run` resolve without sourcing `setenv`).
- INET **4.5** at `/Users/bent/Desktop/Praktikum_Abgabe/Source/inet4.5`, built in **debug**
  mode only (`libINET_dbg.dylib` under `out/clang-debug/src`) — no release build yet.

`sdnproject/sdn.sh` wraps `opp_makemake`/`make`/`opp_run`; its `INET_DIR`/`OMNETPP_DIR`
defaults now point at the paths above (override via env vars if needed). Fixes applied
to the script so it actually works on this machine:
- Library extension: it only searched for `libINET.so`/`libINET_dbg.so` (Linux); added
  `lib_ext()` to pick `.dylib` on macOS (`uname -s`), matching INET's built artifact
  `libINET_dbg.dylib`.
- Build directory: it used to run `opp_makemake` from the project root, which leaves
  `PROJECTRELATIVE_PATH` empty and produces a broken Makefile (`make` can't match its
  own pattern rules — "No rule to make target"). Fixed to build from `src/` instead,
  matching this project's own `.oppbuildspec`/checked-in `src/Makefile` convention (also
  what the Eclipse OMNeT++ IDE does) — the executable now lands at
  `sdnproject/src/sdnproject[_dbg]`.
- Make version: **macOS ships GNU Make 3.81** (`/usr/bin/make`, last GPLv2 release,
  2006) as the default `make`. OMNeT++ 6.3's generated Makefiles use `$(file ...)`,
  added only in GNU Make 4.0 — on 3.81 it silently no-ops, so the `.last-copts`
  prerequisite file is never created and every `.o` target fails with "No rule to make
  target". Fixed by having the script use `gmake` (Homebrew's modern GNU Make, 4.4.1
  here, at `/opt/homebrew/bin/gmake`) instead of `make` whenever available on Darwin.
  **If `gmake` isn't installed, `brew install make` first.**

Verified end-to-end: `./sdn.sh build debug` and `./sdn.sh run BaselineStatic debug`
both succeed and produce `simulations/results/BaselineStatic-#0.{sca,vec}`.

```
cd sdnproject
./sdn.sh build [debug|release]
./sdn.sh run [ConfigName] [debug|release]   # default config: BaselineStatic
./sdn.sh run-all [debug|release]            # BaselineStatic, Dynamic, LowLoad, HighLoad, HotspotTraffic, LinkFailure
./sdn.sh clean
```
