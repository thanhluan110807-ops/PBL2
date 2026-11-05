# Patient CSV Reader Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Load all Patient records from a version-1 CSV file into a new Registry without exposing partial data.

**Architecture:** Concrete `PatientFileManager` returns an owning `PatientRegistry*`, as the user requested. It deletes the temporary Registry on every read error; callers delete successful results after dependent services. An internal CSV parser handles quoted commas, doubled quotes and multiline fields; record conversion validates the fixed schema and uses the current Patient/Registry APIs. The user later removed the abstract reader interface.

**Tech Stack:** C++17 standard file/ownership utilities, project DSA and models, CMake/CTest assertions.

**Spec:** `docs/superpowers/specs/2026-11-04-patient-csv-read-design.md`

## Global Constraints

- Preserve every existing tracked/untracked change; do not commit or push without request.
- Read only Patient CSV; do not add Visit/queue loading, file writing, GUI, or doctor search.
- Code and comments follow `CLAUDE.md`; `My_stdexcept`, no Qt or external parser library.
- The fixed header identifies schema v1; invalid input fails with record start line and leaves any active Registry unchanged.

## Review Focus

- Empty age versus numeric zero, absent birthdate, and leading-zero CCCD remain distinct.
- CSV quoted comma/quote/CRLF and UTF-8 BOM do not corrupt a field or line number.
- Duplicate Patient ID or nonempty CCCD, bad header/column count/enum/date/age, missing file, and unterminated quote report errors.
- Parser never returns a partially loaded Registry.
- New Registry ownership and any borrowed pointers remain clear to callers.

---

### Task 1: CSV parsing and Patient loading

**Files:**
- Create: `include/repository/PatientFileManager.h`
- Create: `src/repository/PatientFileManager.cpp`
- Create: `tests/repository/test_patient_file_manager.cpp`
- Modify: `CMakeLists.txt`
- Modify: `README.md`
- Modify: `docs/repository/README.md`

**Interfaces:**
- Produce: `PatientFileManager::DocBenhNhan(const std::string&) const -> PatientRegistry*`.
- Consume: `PatientRegistry::ThemBenhNhan(const Patient&)`, Patient setters/getters and `My_stdexcept`.

- [x] Write a failing integration test that loads the approved example with all Patient fields, an anonymous Patient, zero age, quoted text and BOM; the later user correction calls the concrete class directly.
- [x] Run the test and observe failure from missing reader API.
- [x] Implement the minimal header/source, strict CSV record parser, field conversion and temporary Registry ownership.
- [x] Add failure fixtures for missing file, bad header/columns, malformed quoting, invalid enum/date/age, and duplicate ID/CCCD; confirm each rejects and does not produce a Registry.
- [x] Register source/test with CMake; run focused test, full Debug build including independent headers, then full CTest.
- [x] Update user-facing repository docs/README to describe the CSV contract and reader's limitations. Review changes/status without committing or pushing.
