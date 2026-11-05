# VisitSearchService Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Expose existing Visit search by patient through the service layer without duplicating search rules.

**Architecture:** `VisitSearchService` borrows a const `PatientRegistry` and forwards the three existing filters to `TraCuuLanKham`. Registry continues to own Patient and Visit and determine matching and order. No staff, file, or GUI dependency.

**Tech Stack:** C++17, project `Vector`, CMake/CTest, assert tests.

**Spec:** `docs/superpowers/specs/2026-11-04-visit-search-patient-design.md`

## Global Constraints

- Keep current names and behavior of `PatientRegistry::TraCuuLanKham`.
- Only search Visit by patient name, optional admission date and exact Visit ID.
- No future doctor-search proposal, `StaffRegistry`, `SearchMode`, file I/O, or GUI.
- Preserve existing files and untracked design document; do not commit or push without request.

## Review Focus

- With all filters empty, return all Visit and no fabricated Patient-only row.
- With combined filters, preserve AND semantics and Registry ordering.
- Invalid date must keep the Registry error behavior.
- Result pointers remain borrowed from Registry, whose lifetime must exceed service's.

---

### Task 1: Add patient-based Visit search service

**Files:**
- Create: `include/service/VisitSearchService.h`
- Create: `src/service/VisitSearchService.cpp`
- Create: `tests/service/test_visit_search_service.cpp`
- Modify: `CMakeLists.txt`
- Modify: `README.md`
- Modify: `docs/superpowers/specs/2026-11-04-visit-search-patient-design.md` (replace design-only status with implemented-status note)

**Interfaces:**
- Consumes: `PatientRegistry::TraCuuLanKham(const std::string&, const Date*, const std::string&) const`.
- Produces: `VisitSearchService(const PatientRegistry&)` and `Vector<KetQuaTraCuuLanKham> TimTheoBenhNhan(const std::string& = "", const Date* = nullptr, const std::string& = "") const`.

- [x] Write an integration test using a real Registry with two same-name Patients, visits at distinct times, an unrelated Patient, and a Patient without Visit. Assert the service returns the expected Visit IDs and Patient IDs for empty, name, combined date/ID, no-match, and invalid-date calls.
- [x] Compile the focused test to observe failure because `VisitSearchService.h` does not exist.
- [x] Add the minimal header/source forwarding implementation and CMake registrations. Keep direct includes and Vietnamese ownership comments.
- [x] Run the focused test, then full Debug CTest suite and source/header checks through the normal build.
- [x] Update README scope/test list and mark the spec's design-only note as implemented. Check Git diff/status, without committing or pushing.
