@echo off
HFST_INSTALLATION_DIR\hfst-open-input-file-for-tagger.exe %* | python HFST_INSTALLATION_DIR\hfst_tagger_compute_data_statistics.py %* | HFST_INSTALLATION_DIR\hfst-build-tagger.exe %*
