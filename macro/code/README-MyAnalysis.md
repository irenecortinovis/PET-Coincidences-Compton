**Input:**
``.root`` file from simulation in Gate: 2 plates (40x40 crystals) facing each other and rotating around a back-to-back gamma source.

File contains ``Hits``, ``Singles`` and ``realCoincidences`` TTrees.

**Output:**
``.root`` file with ``realCoincidences`` TTree filled with inter-crystal Compton events coincidences.



The **aim** is to add the new ``realCoincidences`` to the original ones, analyse them with the reconstruction algorithm and see the impact on spatial resolution and sensitivity of the reconstructed image of the source.



*____________________________________________________*


**``Hits::FindICcoincidences``**

*Find new coincidences with inter-crystals Compton effect, analysing the* ``Hits`` *TTree*.

**``ICCoincidences::FillICCompton``**

*Fill a new* ``realCoincidences`` *TTree with the inter-crystals compton coincidences found in* ``FindICcoincidences``.

**``realCoincidences::FindIDs``**

*Retrieve number of original* ``realCoincidences``*, so it is possible to know and maximise the ratio added/original coincidences*

*____________________________________________________*

**Detail on ``Hits::FindICcoincidences``**

Struct ``Event`` contains useful info from all the ``Hits`` with the same eventID, organized in vectors.

All events are organized in a vector of structs ``Event`` (``events_vector``).

The eventIDs of the events which pass all the requirements to be considered an inter-crystals compton event are stored in ``ICcomptonEvents_vector``.

Struct ``CoincidenceEvent`` contains all the info needed to the reconstruction algorithm in the ``realCoincidences`` TTree (the rest of the TTree will be filled with default values). The struct is filled with the new coincidences with the inter-crystals compton events, so they can be added to the original ``realCoincidences`` TTree.

  The coincidences with inter-crystals compton which pass all the requirements to be considered a ``realCoincidence`` are stored in a vector of structs ``CoincidenceEvent`` (``coincidences_vector``).

**Loop on entries (****Hits****):**

ANALYSE A SINGLE ``HIT``
* Pushback a new struct ``Event`` if the Hit has a new eventID

* Pushback current ``Hit`` info to the vectors in the struct of the current event

* Increase number of different crystals in each rsector if

    * processName is compton OR photoelectric

    * PDGEncoding is gamma (22)

    * energy deposited is > threshold

    * Nnw crystalID in the event

ANALYSE A FINISHED EVENT

* When an event is finished, pushback its eventID in the ``ICcomptonEvents_vector`` if

    * When PDGEncoding == 22 and energy deposited is > threshold, the event has

        * Exactly 2 different crystals in at one rsector, and 1 or 2 in the other rsector

        * processNames are Compton AND photoelectric

****Loop on inter-crystals compton events:****

* Fill ``CoincidenceEvent`` struct (``this_coincidence``) with

    * EventIDs

    * Rotation angle

    * Energy deposited in crystal 0.511 MeV

    * N of Compton phantom 

    * Total energy deposited for each rsector

    * Find first interaction per sector and fill with

        * Their times

        * Their crystal IDs

        * Their X and Y positions

* Fill another ``CoincidenceEvent`` struct (``this_coincidence_incorrect``) with the same info but for one of the two rsectors use info on second interaction (in different crystal, with energy deposited > threshold) instead of first, if, for ``this_coincidence``:

    * Difference of times of interaction is < timeWindow

    * Total energy deposited in each rsector is 0.511 +- sigma

    * Energy deposited in first interaction in each rsector is > threshold

* Pushback ``this_coincidence`` struct to ``coincidences_vector`` and ``this_coincidence_incorrect`` struct to ``incorrect_coincidences_vector`` if ``this_coincidence_incorrect`` satisfies the same conditions just stated before



**Return ``coincidences_vector``**