# Patient/Visit automatic persistence implementation plan

Spec: docs/superpowers/specs/2026-11-04-patient-visit-persistence-design.md

- [x] Shared CSV + Patient writer, UTF8 paths, quote/newline/age0 round-trip.
- [x] Validated Visit snapshots and atomic Registry restore; six statuses/history.
- [x] Triage queue IDs, FIFO and tie metadata export/import.
- [x] Visit CSV full histories, staging validation and no partial import.
- [x] Direct selected-file autosave, raw backups, pending journal, native fsync/replace.
- [x] GUI automatic saving, no manual-savebutton, error/retry/blocking and auto reload.
- [x] Durable selected-path configuration and single-instance lock.
- [x] Failure/SIGKILL tests, strict source checks, independent review and fixes.

User chose direct selected Patient/Visit updates WITH backup, replacing initial
manual-save and private-store proposals. No additional approval gate for explicit
autosave request. No auto-ID or GUI-class restructure. User subsequently authorized commit/push to work.

Verification: Qt5 Debug21/21; ASan/UBSan21/21 detect_leaks=0. Qt6.11.2 syntax
compile all active UI/main/widget tests passed; Qt6 runtime and Windows native
not tested. SIGKILL tested before journal/after journal/after only Patient.
Actual write-limit and fsync failures tested. Independent review identified
first-save failure dead-end; regression failed first, then fixed with separate
committed marker and original-presence check; full suites passed. UTF8 reader
paths now use filesystem::u8path consistently with writers.

No absolute physical-powerloss claim: write before acknowledgement and native
sync depend on filesystem/hardware honoring durability. Prior backups retained.
