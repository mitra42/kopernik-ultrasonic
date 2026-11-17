#!/usr/bin/env python3
"""Draws docs/wiring_supermini.png and docs/wiring_c3_oled.png (and .svg) - run from anywhere.
Uses the wiring-diagrams skill in ~/.claude/skills/wiring-diagrams."""
import os, sys
sys.path.insert(0, os.path.expanduser("~/.claude/skills/wiring-diagrams/scripts"))
from wiring import Diagram
HERE = os.path.dirname(os.path.abspath(__file__))
CHECK = "Check the labels on your module - the header order varies"
TITLE = "SR04M-2 / AJ-SR04M"

# The module sits to the LEFT of the board with its header facing right, reading 5V, RX (Trig),
# TX (Echo), GND top to bottom, and is powered from 3V3 so the echo goes straight to the pin.
# Both boards have trigger 21 BELOW echo 20, the opposite of the module, so those two cross once.
# The module's pins sit half a pitch below the board's, so the two jogs do not share a line.
# GND reaches the bottom of the header from below, round the board, crossing nothing.

# ---- ESP32-C3 SuperMini: 3.3 and G are on the far (right) side among the battery wiring, which
# is as kopernik-soil's. G drops down the right and comes under the board. 3.3 sits between G and
# 4, so it leaves upwards, crossing the 5V and ground wires, and comes over the top to VCC.
d = Diagram(1500, 1060, "kopernik-ultrasonic on an ESP32-C3 SuperMini",
            "GPIO numbers are printed on the back of the board - shown here beside each pin")
b = d.board("esp32c3_supermini", x=680, y=270, width=380, pin_labels=True)
xr, vy, gy, fy = b["5V"][0], b["5V"][1], b["G"][1], b["4"][1]
p = b["G"][1] - b["5V"][1]
y20 = b["20"][1]
M5 = y20 - p / 2                      # the module's top pin
h = max(150, 3 * p + 90)
m = d.ultrasonic_module(x=150, y=M5 - h / 2 + 1.5 * p, pitch=p, title=TITLE)
bottom = b.box[1] + b.box[3] + 50
top = b.box[1] - 40
cell = d.lipo(x=xr + 150, y=vy - 210, leads="bottom")
d.wire(b["5V"], cell["+"], "power", via=["x=" + str(cell["+"][0])])
d.wire(d.junction(cell["-"][0], gy), cell["-"], "ground")
d.wire(b["G"], m["GND"], "ground", via=["x=" + str(xr + 320), "y=" + str(bottom), "x=520"])
d.wire(b["3.3"], m["5V"], "power", via=["x=" + str(xr + 50), "y=" + str(top), "x=650"],
       label="3.3 powers the module", label_at=2)
d.wire(b["21"], m["RX"], "signal", via=["x=625", "y=" + str(m["RX"][1])])
d.wire(b["20"], m["TX"], "signal2", via=["x=575", "y=" + str(m["TX"][1])])
# Battery divider, as kopernik-soil: tap on 4; one 100k to the ground wire, one to the 5V wire
T = d.junction(xr + 95, fy)
d.wire(b["4"], T, "signal3")
d.resistor(xr + 95, fy, xr + 95, gy, "100k")
d.junction(xr + 95, gy)
d.wire(T, (xr + 175, fy), "#9a9a9a", width=3)
d.resistor(xr + 175, fy, xr + 175, gy + 10, "100k")
d.wire((xr + 175, gy + 10), (xr + 175, vy), "power")
d.junction(xr + 175, vy)
d.note(xr + 105, fy + 34, "4 reads the battery\nthrough the two 100k", size=13, colour="#9467bd")
d.note(150, m.box[1] - 40, "21 triggers it, 20 gets the echo", size=13, colour="#2ca02c")
d.note(150, m.box[1] - 18, "At 3.3V the echo needs no divider", size=13, colour="#d62728")
d.note(m.box[0] + 100, m.box[1] + m.box[3] + 40, CHECK, size=13, bold=True)
print(d.save(os.path.join(HERE, "wiring_supermini")))

# ---- ESP32-C3 with the 72x40 OLED, no battery: every pin it needs is on the left, down which
# the board prints V5 GD V3 RX TX. V3 is straight across to VCC; GD is above it and wants the
# bottom pin, so it goes up and over the top, down the unused right side and under the board.
d = Diagram(1320, 860, "kopernik-ultrasonic on the ESP32-C3 OLED 72x40 board",
            "TX is GPIO21 (trigger out), RX is GPIO20 (echo in). No battery on this board")
b = d.board("esp32c3_oled_72x40", x=820, y=170, width=380)
p = b["GD"][1] - b["V5"][1]
M5 = b["V3"][1] + p / 2
h = max(150, 3 * p + 90)
m = d.ultrasonic_module(x=156, y=M5 - h / 2 + 1.5 * p, pitch=p, title=TITLE)
right = b.box[0] + b.box[2] + 40
d.wire(b["V3"], m["5V"], "power", via=["x=720", "y=" + str(M5)], label="V3 powers it", label_at=2)
d.wire(b["TX"], m["RX"], "signal", via=["x=760", "y=" + str(m["RX"][1])])
d.wire(b["RX"], m["TX"], "signal2", via=["x=680", "y=" + str(m["TX"][1])])
d.wire(b["GD"], m["GND"], "ground", via=["x=790", "y=" + str(b.box[1] - 40), "x=" + str(right),
       "y=" + str(b.box[1] + b.box[3] + 40), "x=610"])
d.note(156, m.box[1] - 40, "At 3.3V the echo needs no divider", size=13, colour="#d62728")
d.note(m.box[0], m.box[1] + m.box[3] + 40, CHECK, size=13, bold=True)
print(d.save(os.path.join(HERE, "wiring_c3_oled")))
