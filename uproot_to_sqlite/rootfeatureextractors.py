
from typing import Any, Dict, List, Optional, Union

import numpy as np
import pandas as pd
import awkward as ak

from rootextractor import rootExtractor

class rootFeatureExtractor(rootExtractor):
    """Base class for extracting specific, reconstructed features."""

    def __init__(self, name: str, branch_key: str, feature_keys: List[str]):
        """Construct rootFeatureExtractor.

        Args:
            pulsemap: Name of the pulse (series) map for which to extract
                reconstructed features.
        """
        # Member variable(s)
        self._branch_key = branch_key
        self._feature_keys = feature_keys

        # Base class constructor
        super().__init__(name)


class rootFeatureExtractorESSnuSB(rootFeatureExtractor):
    """Class for extracting reconstructed features for ESSnuSB."""

    def __init__(self, name: str, branch_key: str, feature_keys: List[str], var_dict: Optional[Dict] = {None: None}):

        self._var_dict = var_dict

        # Base class constructor
        super().__init__(name, branch_key, feature_keys)

    def __call__(self, events: "root.events", index: int) -> pd.DataFrame:
        """Extract reconstructed features from `frame`.

        Args:
            events: root events from which to extract reconstructed
                features.

        Returns:
            Dictionary of reconstructed features for all pulses in `pulsemap`,
                in pure-python format.
        """

        hits = events[self._branch_key][self._branch_key+'.'+self._feature_keys[0]].array(library='ak')
        indexes = index + np.arange(len(events))

        repeat_indexes = np.repeat(
            indexes, 
            ak.count(hits, axis=1)
        )

        hit_data = pd.DataFrame(repeat_indexes, columns=['event_no'])

        for feature_key in self._feature_keys:
            hit_data[self._var_dict.get(
                feature_key, feature_key
            )] = ak.flatten(
                events[self._branch_key][self._branch_key+'.'+feature_key].array(library='ak')
            )

        return hit_data
    
class rootTruthExtractorESSnuSB(rootFeatureExtractor):
    """Class for extracting reconstructed features for ESSnuSB."""

    def __init__(self, name: str, branch_key: str, feature_keys: List[str], var_dict: Optional[Dict] = {None: None}):

        self._var_dict = var_dict

        # Base class constructor
        super().__init__(name, branch_key, feature_keys)

    def __call__(self, events: "root.events", index: int) -> pd.DataFrame:
        """Extract reconstructed features from `frame`.

        Args:
            events: root events from which to extract reconstructed
                features.

        Returns:
            Dictionary of reconstructed features for all pulses in `pulsemap`,
                in pure-python format.
        """

        indexes = index + np.arange(len(events))

        # Get truths
        truth_data = pd.DataFrame(indexes, columns=['event_no'])
        for feature_key in self._feature_keys:
            truth_data[self._var_dict.get(
                    feature_key, feature_key
                    )] = events[self._branch_key][feature_key].array(library='ak')

        return truth_data
     
class rootfiTQunExtractorESSnuSB(rootFeatureExtractor):
    """Class for extracting reconstructed features for ESSnuSB."""

    def __init__(self, name: str, branch_key: str, feature_keys: List[str], var_dict: Optional[Dict] = {None: None}):

        self._var_dict = var_dict

        # Base class constructor
        super().__init__(name, branch_key, feature_keys)

    def __call__(self, events: "root.events", index: int) -> pd.DataFrame:
        """Extract reconstructed features from `frame`.

        Args:
            events: root events from which to extract reconstructed
                features.

        Returns:
            Dictionary of reconstructed features for all pulses in `pulsemap`,
                in pure-python format.
        """

        indexes = index + np.arange(len(events))

        # Get truths
        fiTQun_data = pd.DataFrame(indexes, columns=['event_no'])
        for feature_key in self._feature_keys:
            fiTQun_data[self._var_dict.get(
                    feature_key, feature_key
                    )] = events[self._branch_key][feature_key].array(library='ak')

        return fiTQun_data