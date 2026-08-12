# Real application data provenance

## Authoritative record

- **Title:** Raw data: Observation of two-mode squeezing in a traveling wave parametric amplifier
- **Creators:** Martina Esposito, Arpit Ranadive, Nicolas Roch
- **Repository:** Zenodo
- **Record:** https://zenodo.org/records/5217997
- **DOI:** https://doi.org/10.5281/zenodo.5217997
- **Published:** 2021-11-08, version v1
- **License shown by Zenodo:** MIT License
- **Total archive size shown by Zenodo:** 25.6 GB
- **Archive name:** `Raw_data_2_mode_squeezing`
- **Archive MD5 shown by Zenodo:** `59590d1cdf5fead5283493764164c9f1`

## Relevant measured data

Zenodo describes the records as QCoDeS datasets. For the pump-phase sweep used for the squeezed-microwave application, the record states that the sweep contains 25 points from 0 to pi and is stored in database run IDs 11 through 35, in order. The complete archive also contains other figure data and is therefore much larger than the needed subset.

The stakeholder directive narrows the application to reading a sample of 100,000 measured values at each of those 25 pump-phase settings, computing the measured noise at each setting, and recommending the phase with the minimum measured variance.

## Evidence boundary

The source is real measured quantum-device data, not simulated data. The application may establish a measured-data calibration result. It must not be described as a new physical law or as proof that QAU is a fundamental physical unit. The present stakeholder result reports a default phase of 0 radians and a recommended phase near 2.094 radians (run 27), with a 1.82% measured variance reduction and a positive confidence interval. Those values must be independently reproduced from the raw records rather than copied as an unverified claim.

## Local browser evidence

The Zenodo page HTML was saved by the browser to `/home/ubuntu/browser_html/zenodo_org_5217997_1786537782423.html` during provenance inspection. The page markdown was saved to `/home/ubuntu/page_texts/zenodo.org_records_5217997.md`.

## API verification

The Zenodo REST API confirms a single archive file with size `25,626,624,000` bytes and checksum `md5:59590d1cdf5fead5283493764164c9f1`. Its content endpoint is `https://zenodo.org/api/records/5217997/files/Raw_data_2_mode_squeezing/content`. The archive is not a small CSV; a deployable application must therefore support a local dataset path and a documented acquisition/extraction step, and the repository should not silently download 25.6 GB during a normal build.

The page exposes the file URL `/records/5217997/files/Raw_data_2_mode_squeezing?download=1`. The archive’s internal QCoDeS structure still needs to be inspected after acquisition of the relevant subset or a user-supplied local copy.
