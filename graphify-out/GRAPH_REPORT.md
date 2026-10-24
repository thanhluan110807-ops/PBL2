# Graph Report - PBL2  (2026-10-22)

## Corpus Check
- Corpus is ~7,998 words - fits in a single context window. You may not need a graph.

## Summary
- 174 nodes · 242 edges · 11 communities (8 shown, 3 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 11 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Patient & Triage Main
- Queue Pressure Metrics
- Priority Queue Heap
- Pressure Calculator Logic
- Person Base Class
- Headers & Std Includes
- Custom String
- Patient File Manager
- Custom Vector
- Dev Notes

## God Nodes (most connected - your core abstractions)
1. `Patient` - 42 edges
2. `QueuePressureCalculator` - 22 edges
3. `QueuePressureReport` - 20 edges
4. `Person` - 16 edges
5. `PriorityQueue` - 13 edges
6. `Vector` - 13 edges
7. `my_string` - 11 edges
8. `PressureLevel` - 6 edges
9. `writePatientCsvRow()` - 5 edges
10. `PatientFileManager` - 5 edges

## Surprising Connections (you probably didn't know these)
- `Patient` --inherits--> `Person`  [EXTRACTED]
  Patient.h → Person.h
- `writePatientCsvRow()` --references--> `Patient`  [EXTRACTED]
  PatientFileManager.cpp → Patient.h
- `my_string::my_string()` --calls--> `length`  [EXTRACTED]
  my_string.cpp → my_string.h

## Import Cycles
- None detected.

## Communities (11 total, 3 thin omitted)

### Community 0 - "Patient & Triage Main"
Cohesion: 0.06
Nodes (15): main(), Patient, gcsScore, heartRate, isAnonymous, isReentrant, nextStt, Patient::Patient() (+7 more)

### Community 1 - "Queue Pressure Metrics"
Cohesion: 0.08
Nodes (24): PressureLevel, CRITICAL_OVERLOAD, ELEVATED, HIGH_LOAD, NORMAL, QueuePressureReport, anonymousCount, averageWaitingTime (+16 more)

### Community 2 - "Priority Queue Heap"
Cohesion: 0.11
Nodes (7): PriorityQueue, capacity, heap, size, PriorityQueue<T>::peek(), PriorityQueue<T>::pop(), PriorityQueue<T>::push()

### Community 3 - "Pressure Calculator Logic"
Cohesion: 0.09
Nodes (10): QueuePressureCalculator, DOCTOR_CAPACITY_UNIT, MAX_BENCHMARK_PRESSURE, NURSE_CAPACITY_UNIT, REENTRANT_BONUS_WEIGHT, WEIGHT_ESI_1, WEIGHT_ESI_2, WEIGHT_ESI_3 (+2 more)

### Community 4 - "Person Base Class"
Cohesion: 0.20
Nodes (6): Person, age, fullName, gender, id, Person::Person()

### Community 6 - "Custom String"
Cohesion: 0.20
Nodes (5): my_string, my_string_copy(), data, length, my_string::my_string()

### Community 7 - "Patient File Manager"
Cohesion: 0.33
Nodes (3): looksLikeHeader(), PatientFileManager, writePatientCsvRow()

### Community 8 - "Custom Vector"
Cohesion: 0.20
Nodes (4): Vector, capacity, count, data

## Knowledge Gaps
- **55 isolated node(s):** `heartRate`, `systolicBP`, `respiratoryRate`, `spO2`, `gcsScore` (+50 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 110 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **3 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Patient` connect `Patient & Triage Main` to `Person Base Class`, `Headers & Std Includes`, `Patient File Manager`?**
  _High betweenness centrality (0.365) - this node is a cross-community bridge._
- **What connects `heartRate`, `systolicBP`, `respiratoryRate` to the rest of the system?**
  _55 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Patient & Triage Main` be split into smaller, more focused modules?**
  _Cohesion score 0.06349206349206349 - nodes in this community are weakly interconnected._
- **Why does `QueuePressureCalculator` connect `Pressure Calculator Logic` to `Headers & Std Includes`?**
  _High betweenness centrality (0.207) - this node is a cross-community bridge._
- **Should `Queue Pressure Metrics` be split into smaller, more focused modules?**
  _Cohesion score 0.08333333333333333 - nodes in this community are weakly interconnected._
- **Why does `QueuePressureReport` connect `Queue Pressure Metrics` to `Headers & Std Includes`?**
  _High betweenness centrality (0.180) - this node is a cross-community bridge._
- **Should `Priority Queue Heap` be split into smaller, more focused modules?**
  _Cohesion score 0.1067193675889328 - nodes in this community are weakly interconnected._