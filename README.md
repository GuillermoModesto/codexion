*This project has been created as part of the 42 curriculum by guantino.*

# Codexion

## Description

Codexion simulates a shared co-working hub where several people write code and compete
for a limited pool of USB dongles. Compiling requires holding two dongles at the same
time (the one on the left and the one on the right), so neighbours are permanently in
competition for the same hardware.

Each coder is a POSIX thread that cycles through three phases: compiling, debugging and
refactoring. A coder who does not manage to start compiling within `time_to_burnout`
milliseconds of the start of its previous compile (or of the start of the simulation)
burns out, and the simulation stops. The simulation also stops, successfully, once every
coder has compiled at least `number_of_compiles_required` times.

The goal of the project is to orchestrate this competition correctly: no deadlock, no
starvation, no duplicated dongle, no interleaved log line, and burnout detected within
10 ms of the moment it actually happens.

Beyond the classic version of this problem, Codexion adds two constraints: a released
dongle stays unavailable for a cooldown period, and the arbitration between competing
coders follows an explicit scheduling policy (`fifo` or `edf`) implemented on top of a
binary min-heap priority queue.

## Instructions

Compile:

```
make
```

The Makefile provides the `all`, `clean`, `fclean` and `re` rules and builds with
`-std=c89 -Wall -Wextra -Werror -pthread`.

Run:

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All eight arguments are mandatory. The seven numeric ones are positive integers in
milliseconds (except `number_of_coders` and `number_of_compiles_required`, which are
counts), and `scheduler` must be exactly `fifo` or `edf`. Invalid input (negative
numbers, non-integers, zero coders, an unknown scheduler, a wrong argument count) is
rejected with an error message and a non-zero exit status.

Example:

```
./codexion 5 1500 150 80 80 3 20 edf
```

Output format, one state change per line:

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Timestamps are relative to the start of the simulation, and coders are numbered from 1
to `number_of_coders`.

## Usage examples

```
./codexion 5 1500 150 80 80 3 20 edf     # completes, nobody burns out
./codexion 5 1500 150 80 80 3 20 fifo    # same, with FIFO arbitration
./codexion 1 500 200 200 200 3 30 edf    # a single coder has a single dongle: burns out
./codexion 200 8000 60 60 60 2 60 edf    # large scale
./codexion 5 100 300 60 60 3 60 edf      # infeasible parameters: burns out under both
```

## Features

- Each coder is an independent POSIX thread.
- A separate monitor thread detects burnout and stops the simulation.
- Two arbitration policies, `fifo` and `edf`, on a generic binary min-heap.
- Mandatory dongle cooldown after every release.
- Serialized logging with millisecond timestamps.
- No global variables: all shared state lives in a struct passed by pointer.
- No memory leaks; every mutex and condition variable is destroyed.

## Technical decisions

**Deadlock avoidance by resource ordering.** Each coder always requests the
lower-indexed of its two dongles first. This is what breaks the circular wait (see
below).

**Cooldown checked on demand.** Nothing in the program ever writes `DONGLE_FREE` back
after a release; a dongle released goes to `DONGLE_COOLDOWN` with a timestamp, and
whoever wants it checks whether the cooldown has actually elapsed. This avoids needing a
separate thread or timer per dongle just to flip a state back.

**The scheduler decision is made once.** The priority function (`fifo_priority` or
`edf_priority`) is chosen at initialisation and handed to each heap as a function
pointer. No code past `init_sim` ever branches on the scheduler string again.

**EDF priority is `compile_timer`, not `compile_timer + time_to_burnout`.** The subject
defines the deadline as `last_compile_start + time_to_burnout`, but `time_to_burnout` is
the same constant for every coder, so adding it to all values does not change their
relative order. Sorting by `compile_timer` produces exactly the same queue.

**Sleeping in small chunks.** Coders sleep in 0.5 ms slices while checking the stop
flag, and the monitor polls every 1 ms. This costs a small amount of CPU but is what
allows the simulation to stop promptly and the burnout log to stay well inside the 10 ms
requirement (measured: 1-2 ms).

## Blocking cases handled

**Deadlock (circular wait).** The textbook failure of this problem is every coder taking
its left dongle and then waiting for its right one: each holds one and waits for the
neighbour's, closing a cycle. Of the four Coffman conditions (mutual exclusion,
hold-and-wait, no preemption, circular wait), this implementation breaks **circular
wait** by imposing a total order on resources: every coder requests the lower-indexed
dongle of its pair first. For coder *i* the two dongles are `(i-1) mod N` and `i`, so the
subtraction only wraps around for coder 0, which therefore requests its dongles in the
opposite order to everyone else. That single asymmetry is enough to make the cycle
impossible for any N.

**Starvation.** Without an explicit arbitration policy, which coder wins a contended
dongle depends on the operating system's scheduling, and one coder could lose
systematically and burn out. Every dongle owns a priority queue (binary min-heap) of the
coders currently waiting for it, and a waiting coder only proceeds when it is the minimum
of that heap. Under `fifo` the key is the timestamp at which the request was made; under
`edf` it is the coder's burnout deadline.

Note on the scope of arbitration: a dongle can only ever be requested by its two adjacent
coders, and a coder that is *holding* a dongle is not in the heap. Two coders therefore
only coexist in the same heap while the dongle is in cooldown, which is the window in
which the scheduler actually arbitrates. This was verified by instrumenting the heap:
with a small cooldown, contention events are rare; with `dongle_cooldown 300`, 25-35
arbitration events occur per run.

**Cooldown management.** A released dongle is marked `DONGLE_COOLDOWN` together with the
timestamp of the release. `dongle_available()` treats it as usable only once
`now - state_timer >= dongle_cooldown`. A coder waiting on a dongle in cooldown sleeps
with `pthread_cond_timedwait` until the exact moment the cooldown expires, rather than
polling.

**Busy-waiting.** An earlier version computed the wake-up deadline as
`state_timer + time_to_compile` for a dongle in use. Once that instant had passed but the
holder had not yet released, the deadline stayed fixed in the past,
`pthread_cond_timedwait` returned immediately, and the loop spun. Measured: 33893 of
33936 waits had an already-expired deadline, and the program used 65% CPU. `next_deadline`
now clamps the result to at least `now + 5 ms`, which brought CPU usage down to under 4%
and the number of wait iterations from 38109 to 246 on the same workload.

**Precise burnout detection.** A dedicated monitor thread scans every coder's
`compile_timer` every millisecond and is the only thread that ever sets the stop flag.
Measured delay between the real burnout instant and the printed line: 1-2 ms over 35
runs, against a 10 ms requirement.

**Log serialization.** A dedicated `log_mutex` protects stdout so two messages can never
interleave on one line. The burnout message sets the stop flag and prints inside the same
critical section, so no other message can slip in between; after the flag is set, regular
messages are dropped, guaranteeing `burned out` is the last line. Verified on a 2365-line
run with 200 coders: every line matches the expected format exactly and timestamps are
monotonically non-decreasing.

**Clean shutdown.** A coder asleep in `pthread_cond_timedwait` would not notice the end
of the simulation and `pthread_join` would hang. The wait loop therefore also exits on
the stop flag, and the monitor broadcasts on every dongle's condition variable before
terminating, so all sleepers wake up, re-check, and exit.

## Thread synchronization mechanisms

**`pthread_mutex_t`.** Four families of mutexes, each protecting one kind of data:

- `dongle->mutex`, one per dongle, protects that dongle's state, its release timestamp
  and its waiting heap. Both acquisition and release take it, which is what makes the
  protection meaningful: a mutex protects data, not code, so every access path must
  respect the same agreement.
- `coder->state_mutex`, one per coder, protects `compile_timer` and `compile_amount`.
  These are written by the coder's own thread and read concurrently by the monitor; this
  is the classic lost-update race, and the mutex is what prevents it.
- `sim->log_mutex` protects stdout.
- `sim->stop_mutex` protects the stop flag, read by every thread and written by the
  monitor.

**`pthread_cond_t`.** One condition variable per dongle (`wait_room`). A coder that
cannot proceed sleeps on it instead of polling. Only `pthread_cond_broadcast` is
available in this project, so every waiter wakes on any change and re-evaluates its own
condition in a `while` loop — the decision of whose turn it is is not made by the signal
but by each thread's own check against the heap.

`pthread_cond_timedwait` is used rather than `pthread_cond_wait` because most waits have
a known end: the compile in progress, or the cooldown. The deadline is an absolute
`struct timespec` built from `gettimeofday`, clamped to always lie in the future.

**Avoiding race conditions — example.** The monitor reads `compile_timer` to decide
whether a coder has burned out, while that coder writes it when starting a compile.
Without synchronization the monitor could read a half-updated value. Both sides take
`coder->state_mutex` around the access, so the read is always of a complete, consistent
value. The same pattern applies to the stop flag: `sim_should_stop()` and
`sim_set_stop()` are the only access points and both take `stop_mutex`.

**Thread-safe coder/monitor communication — example.** The monitor never calls into coder
threads directly. It communicates by setting the stop flag (under its mutex) and by
broadcasting on every dongle's condition variable. Coders observe the flag at three
points: between phases in their main loop, inside `sim_sleep`, and inside the dongle wait
loop. This gives a one-way, lock-protected channel with no shared mutable state outside
the mutexes.

**Verification.** The implementation was checked with ThreadSanitizer (no data races
reported, including at 30 coders), AddressSanitizer (no leaks at 100 coders), repeated
runs looking for hangs (no deadlock), and format validation of the complete log.

## Resources

- E. W. Dijkstra, *Hierarchical Ordering of Sequential Processes* (1971) — the original
  Dining Philosophers problem, of which this project is a variant.
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks* (1971) — the four conditions
  for deadlock.
- C. L. Liu and J. W. Layland, *Scheduling Algorithms for Multiprogramming in a
  Hard-Real-Time Environment* (1973) — Earliest Deadline First.
- `man` pages: `pthread_create`, `pthread_mutex_lock`, `pthread_cond_wait`,
  `pthread_cond_timedwait`, `gettimeofday`, `usleep`.
- A. Silberschatz, P. Galvin, G. Gagne, *Operating System Concepts* — chapters on
  synchronization and deadlocks.
- T. Cormen et al., *Introduction to Algorithms* — binary heaps and priority queues.

### Use of AI

<!-- REVISA Y AJUSTA ESTA SECCION: debe reflejar tu experiencia real. -->

AI (Claude) was used throughout this project, in the following way and for the following
parts:

- **Learning the concepts.** Most of the time was spent on guided explanation of threads,
  mutexes, condition variables, race conditions and deadlock, with small standalone demo
  programs written and run to observe each problem happening (a counter losing increments
  without a mutex, a thread burning 99% CPU polling instead of sleeping) and then being
  fixed. These demos are not part of the deliverable.
- **Design discussion.** The data structures, the resource-ordering rule that prevents
  deadlock, the choice of what belongs in each struct, and the decision to implement the
  scheduler as a function pointer chosen once at initialisation were worked out through
  question-and-answer rather than being handed over ready-made.
- **Code.** The generic binary heap (`heap_*.c`) and the time helpers (`time_utils.c`)
  were produced with AI assistance as infrastructure. The synchronization core
  (`dongle_acquire.c`), the coder loop, the monitor and the initialisation/cleanup were
  developed with AI assistance as well, after the design had been reasoned through.
- **Review and debugging.** AI was used to compile, stress-test and instrument the
  program. This is how the busy-waiting bug in `next_deadline` was found and measured, how
  the C89 compliance issue with `usleep` was caught, and how the actual frequency of
  scheduler arbitration was quantified.
- **Not used for:** this README's factual claims were checked against measurements taken
  from the code itself rather than assumed.
