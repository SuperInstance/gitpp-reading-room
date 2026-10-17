# Three green lights that were never connected

Short-form video explainers for the git.pp conformance work. Each one is
a **visual metaphor** — video models render text unreliably, so none of
these put words on screen. The captions live in `index.html`.

| # | file | the idea |
|---|---|---|
| 1 | `01-a-green-light-never-connected.mp4` | A control panel where every lamp glows green except one. Push in on the dark one and the wire behind it is cut. **25 of the 29 conformance cases were compared against nothing** — the comparator was a one-argument function, so it returned a truthy string without ever inspecting the expected value. |
| 2 | `02-131499-boxes-nobody-opened.mp4` | An endless belt of identical sealed boxes. A worker opens exactly one, and it is empty. **The mutation pass generated 131,499 mutants and no runner ever consumed them.** |
| 3 | `03-break-it-and-watch-the-number-move.mp4` | A precision balance, green readout, a brass weight placed deliberately on the pan. The number shifts and the display goes amber. **The negative control** — break the thing on purpose; if the number does not move, the number was never measuring anything. |

## How these were made and verified

Rendered with **MiniMax-H3-Max** (768P, 8s, native audio track) from
prompts that describe the scene and the sound, with no on-screen text.

Every video was then checked by an **independent vision-language pass**
that answered structured questions about the content:

```
v1  45 indicator positions, 5 rows of 9, most green
    a single dark unlit lamp in the centre, camera pushing in on it
    a severed wire with an exposed copper end
    no speech -- ambient hum and mechanical clicks only

v2  warehouse conveyor, worker in a hard hat and orange hi-vis vest
    slices the tape with a utility knife, opens the flaps
    the box is completely EMPTY
    voiceover: "131,000 boxes. And not one of them was ever opened."

v3  precision digital scale on stainless steel, gloved hand
    places a brass calibration weight on the pan
    readout changes 73.3 -> 78.5, backlight green -> amber
    no speech
```

**The captions were written from the VQA transcripts, not from the
prompts.** Where the two differ, the transcript wins — the video-2
voiceover says "131,000" where the prompt said "one hundred and
thirty-one thousand", and the caption quotes the video.

That is the same negative-control discipline the rest of this project
runs on: a generated artefact is not a verified one, and the check has
to come from outside the thing that made it.

## What was NOT verified

- **The page layout is unverified by image.** Chromium SIGTRAPs in this
  sandbox, so no pixel screenshot was possible. `index.html` was
  verified structurally (three `<video>` elements, three captions, all
  three sources resolving HTTP 200) and the *video content* was verified
  by VQA. The layout itself has not been seen rendered.
- The videos carry no subtitles, and only video 2 has speech.

## Files

```
index.html   the three videos with their captions
0*.mp4       the three videos
```

Open `index.html` in a browser; it references the videos by relative
path and works offline.
