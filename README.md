# Beat_Flow
Create music with pressure sensitive midi footwear

View full project at https://marquessa.art/beat-flow

Quick start guide:
  1. Get unique MAC address of receiver board
  2. Identify and select ports for all boards
  3. Update sender to include receiver MAC address
  4. Update any pins or LED requirements
  5. Upload all code + connect to bluetooth
  6. Confirm successful delivery/receipt in serial monitor
  7. Determine threshold readings for all sensors and LEDs
  8. Update values if necessary
  9. Detach sender boards and hook up to appropriate power supply
  10. Open key mapped midi software and place sounds on “r” and “l” keys

Note about threshold organizing principles:
  - Each sender has a threshold that corresponds to attached LEDs and can be customized to match analog values from individual sensors.
  - Values sent to the receiver are to trigger the key mapping midi software and will print out on the display.
  - These can be customized without needing to interfere with the sender.
