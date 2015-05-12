# Design Background #
This project is the server of cbc- series. There may be cbc-client which defines cli, gui, mobile clients, and cbc-data which defines different rules and resources.

  1. Learn advanced software design skills by implementing SanGuoSha elegantly, without any compromise on code quality.
  1. Separate server and client in a net-game, server controls all rules so that user can connect with any client without cheating. The server communication protocol will be made open.
  1. Unix philosophy. The client only defines images, sounds and layouts, all mechanisms should be defined server side.
  1. kdegame style svg graphics, all image must be svg. which ensures high picture quality in high resolution.
  1. retain all games as boardgame instead of a pc game.

# Project Overview #
This is the portal of cbc projects:
  * cbc-server
    * Development WiKi
    * ServerProtocol
  * cbc
    * Game server daemon and control console
  * cbc-cli
    * CLI Client as a demo client
  * cbc-client
    * GUI Game Clients, include desktop and mobile.
  * cbc-data
    * Core Engines and Rules
    * Text, l10n and i18n
    * Music and Image resources
    * plugins, add-ons, DIYs

# Difference between Moligaloo QSanguosha #
  * Sanguosha is both a popular board game and online game,
  * This project try to clone the board game.
  * While QSanguosha try to clone the online game.

# Note #
Every wiki page start with zh-Hans pages, Volunteers are welcome to translate zh-Hans pages to en pages and / or other languages. When in doubt, always consult zh-Hans version of a wiki page.