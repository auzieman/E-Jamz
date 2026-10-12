# LCD font resource

Doto by Óliver Lalan / The Doto Project Authors, under SIL OFL 1.1.
[Upstream](https://github.com/oliverlalan/Doto) and the complete license in
[OFL.txt](OFL.txt). Original Google Fonts source blobs and SHA256 hashes are
recorded in provenance.json; the variable TTF is retained unchanged.

The remote build instantiates weight 800 and roundness 0 with FontTools, then
embeds that square-dot instance in the Edje skin as EJamzLCD. Only the main
track title and elapsed clock use it. No desktop font installation is needed.
The build retains the font and OFL license beside the compiled skin.

Preview: tools/preview-lcd.py uses a project-local Fontconfig configuration to
render the variable font at the chosen weight without changing host fonts.
Native screenshots are runtime evidence; the SVG/PNG is a typography study.
