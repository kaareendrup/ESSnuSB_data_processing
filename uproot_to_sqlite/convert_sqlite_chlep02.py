# %%

from graphnet.data.sqlite.root_dataconverter import rootSQLiteDataConverter
from graphnet.data.extractors.rootfeatureextractor import (
    rootFeatureExtractorESSnuSB,
    rootTruthExtractorESSnuSB,
    rootfiTQunExtractorESSnuSB
)

# %% 

CONVERTER_CLASS = {
    "sqlite": rootSQLiteDataConverter,
}

def main_essnusb(backend: str) -> None:
    """Convert ESSnuSB files to intermediate `backend` format."""
    # Check(s)
    assert backend in CONVERTER_CLASS

    inputs = ["/home/kaare/snic2022-23-570/ESSnuSB/root_events/chlep02"]
    outdir = "/home/kaare/snic2022-23-570/ESSnuSB/databases"
    name = "ESSnuSB_chlep02"

    converter: rootSQLiteDataConverter = CONVERTER_CLASS[backend](
        [   
            rootFeatureExtractorESSnuSB('ESSnuSB_pulses', 'PMTubeHit'),
            rootTruthExtractorESSnuSB('truth', 'event'),
            rootfiTQunExtractorESSnuSB('fiTQun', 'fitqun'),
        ],
        outdir,
        name,
        nb_files_to_batch=20,
        remove_empty_events=True,
    )
    converter(inputs)

# %%

main_essnusb("sqlite")

# %%