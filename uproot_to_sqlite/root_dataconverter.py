# %%

import os
from abc import ABC
from collections import OrderedDict
from typing import List, Union

import uproot

import itertools

from graphnet.utilities.logging import Logger
from graphnet.data.sqlite.sqlite_utilities import (
    create_table_and_save_to_sql,
)
from graphnet.utilities.decorators import final
from graphnet.utilities.filesys import find_root_files

from rootextractor import rootExtractor, rootExtractorCollection
from ESSnuSB.ESSnuSB_data_processing.uproot_to_sqlite.rootfeatureextractors import rootFeatureExtractor

# %%

class rootSQLiteDataConverter(ABC, Logger):

    def __init__(
        self,
        extractors: List[rootExtractor],
        outdir: str,
        *,
        main_key: str = None,
        index_column: str = "event_no",
    ):

        if not isinstance(extractors, (list, tuple)):
            extractors = [extractors]

        assert (
            len(extractors) > 0
        ), "Please specify at least one argument of type rootExtractor"

        for extractor in extractors:
            assert isinstance(
                extractor, rootExtractor
            ), f"{type(extractor)} is not a subclass of rootExtractor"


        # Member variables
        self._outdir = outdir
        self._main_key = main_key

        # Create I3Extractors
        self._extractors = rootExtractorCollection(*extractors)

        # Create shorthand of names of all pulsemaps queried
        self._table_names = [extractor.name for extractor in self._extractors]
        self._pulsemaps = [
            extractor.name
            for extractor in self._extractors
            if isinstance(extractor, rootFeatureExtractor)
        ]

        # Placeholders for keeping track of sequential event indices and output files
        self._index_column = index_column
        self._index = 0
        self._output_files: List[str] = []

        # Base class constructor
        super().__init__(name=__name__, class_name=self.__class__.__name__)


    @final
    def __call__(self, directories: Union[str, List[str]]) -> None:
        """Convert root-files in `directories`.

        Args:
            directories: One or more directories, the root files within which
                should be converted to an intermediate file format.
        """
        # Find all root files in the specified directories.
        root_files = find_root_files(directories)
        if len(root_files) == 0:
            self.error(f"No files found in {directories}.")
            return

        # Save a record of the found root files in the output directory.
        # self._save_filenames(root_files)

        # Process the files
        self.execute(root_files)

    @final
    def execute(self, files: List[str]) -> None:
        """General method for processing a set of root files.

        The files are converted individually according to the inheriting class/
        intermediate file format.

        Args:
            files: List of paths to root files.
        """
        # Make sure output directory exists.
        self.info(f"Saving results to {self._outdir}")
        os.makedirs(self._outdir, exist_ok=True)

        # Iterate over batches of files.
        try:
            self._iterate_over_all_files(files)

        except KeyboardInterrupt:
            self.warning("[ctrl+c] Exciting gracefully.")

    def _iterate_over_all_files(self, files: List[str]) -> None:
        """Iterate over a all files and save results."""

        self._process_batch(files)

    def _process_batch(self, files: List[str]) -> str:

        # Process individual files
        data = list(
            itertools.chain.from_iterable(map(self._extract_data, files))
        )

        # Save batched data
        output_file = self._get_output_file(self._output_file_name)
        self.save_data(data, output_file)

        return output_file

    def _extract_data(self, file: str) -> List[OrderedDict]:
        """Extract data from single root file.

        Args:
            fileset: Path to root file.

        Returns:
            Extracted data.
        """
        if self._main_key is None:
            self._main_key = uproot.open(file).keys()[0]

        self._extractors.set_files(file)

        events = uproot.open(file + ":" + self._main_key)

        # Get new, unique index and increment value
        index = self._index 
        self._index += len(events)

        results = self._extractors(events, index)

        data_dict = OrderedDict(zip(self._table_names, results))

        return data_dict

    def save_data(self, data: List[OrderedDict], output_file: str) -> None:
        """Save data to SQLite database."""
        # Check(s)
        if os.path.exists(output_file):
            self.warning(
                f"Output file {output_file} already exists. Appending."
            )

        # Test data
        if len(data) == 0:
            self.warning(
                "No data was extracted from the processed root file(s). "
                f"No data saved to {output_file}"
            )
            return

        saved_any = False

        for table, df in data.items():
            if len(df) > 0:
                create_table_and_save_to_sql(
                    df,
                    table,
                    output_file,
                    default_type="FLOAT",
                    integer_primary_key=not (
                        is_pulse_map(table) or is_mc_tree(table)
                    ),
                )
                saved_any = True

        if saved_any:
            self.debug("- Done saving")
        else:
            self.warning(f"No data saved to {output_file}")


def is_pulse_map(table_name: str) -> bool:
    """Check whether `table_name` corresponds to a pulse map."""
    return "pulse" in table_name.lower() or "series" in table_name.lower() or "hit" in table_name.lower()


def is_mc_tree(table_name: str) -> bool:
    """Check whether `table_name` corresponds to an MC tree."""
    return "I3MCTree" in table_name

