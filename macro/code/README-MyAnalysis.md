**Input:**
``.root`` file from simulation in Gate: 2 plates (40x40 crystals) facing each other and rotating around a back-to-back gamma source.

File contains ``Hits``, ``Singles`` and ``realCoincidences`` TTrees.

**Output:**
For each percentage of correct inter-crystal events predictions, create a ``.root`` file with ``realCoincidences`` TTree filled with

  * inter-crystal Compton events coincidences, correct or incorrect, according to the percentage

  * the original realCoincidences which are not inter-crystal.

If needed, to save space, it is possible to delete the original file and only save its realCoincidences TTree.



The **aim** is to add the new ``realCoincidences`` to the original ones, analyse them with the reconstruction algorithm and see the impact on spatial resolution and sensitivity of the reconstructed image of the source, varying the percentage of correct inter-crystal predictions.



*____________________________________________________*

**``Hits::FindICcoincidences``**

*Find new coincidences with inter-crystals Compton effect, analysing the* ``Hits`` *TTree*.


**``ICCoincidences::FillICCompton``**

*Fill a new* ``realCoincidences`` *TTree with the inter-crystals compton coincidences found in* ``FindICcoincidences``, according to the percentage.

**``finalCoincidences::MergeTTrees``**

Merge inter-crystals compton coincidences tree with the original realCoincidences ttree (giving priority to the inter-crystals events)

*____________________________________________________*

**Detail on ``Hits::FindICcoincidences``**

Struct ``Event`` contains useful info from all the ``Hits`` with the same eventID, organized in vectors.

All events are organized in a vector of structs ``Event`` (``events_vector``).

The eventIDs of the events which pass all the requirements to be considered an inter-crystals compton event are stored in ``ICcomptonEvents_vector``.

Struct ``CoincidenceEvent`` contains all the info needed to the reconstruction algorithm in the ``realCoincidences`` TTree (the rest of the TTree will be filled with default values). The struct is filled with the new coincidences with the inter-crystals compton events, so they can be added to the original ``realCoincidences`` TTree.

  The coincidences with inter-crystals compton which pass all the requirements to be considered a ``realCoincidence`` are stored in a vector of structs ``CoincidenceEvent`` (``coincidences_vector``).

**Loop on entries (****Hits****):**

ANALYSE A SINGLE HIT
* Pushback a new struct ``Event`` if the Hit has a new eventID

* Pushback current ``Hit`` info to the vectors in the struct of the current event

* Increase number of different crystals hit in each rsector in the current event if all conditions apply:

    * processName is compton OR photoelectric

    * energy deposited in crystal so far is > threshold

    * crystal is not already counted as new crystal


ANALYSE A FINISHED EVENT

* When an event is finished, pushback its eventID in the ``ICcomptonEvents_vector`` if the event has

    * Exactly 2 different crystals in one rsector, and 1 or 2 in the other rsector

    * processNames are Compton AND photoelectric in the rsector where the inter-crystal interaction happened

****Loop on inter-crystals compton events:****

* Fill ``CoincidenceEvent`` struct (``this_coincidence``) with

    * EventIDs

    * Rotation angle

    * Energy deposited in crystal 0.511 MeV

    * N of Compton phantom

    * Total energy deposited for each rsector (511 keV)

    * Find first interaction per sector and fill with

        * Their times

        * Their crystal IDs

        * Their X and Y positions

* Fill another ``CoincidenceEvent`` struct (``this_coincidence_incorrect``) with the same info but for one of the two rsectors use info on second interaction (in different crystal, with energy deposited > threshold) instead of first.

* Check whether for both ``this_coincidence`` and ``this_coincidence_incorrect``:

    * Difference of times of interaction is < timeWindow

    * Total energy deposited in each rsector is 0.511 +- sigma

    * The energy depositions considered in the coincidence had increased the number of crystals hit in rsector (see filters before)

* If so, pushback ``this_coincidence`` struct to ``coincidences_vector`` and ``this_coincidence_incorrect`` struct to ``incorrect_coincidences_vector``

* Pushback eventID in the eventIDs vector



**Return ``coincidences_vector``**
