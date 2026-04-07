// Elechouse NFC Module V3 (PN532) Mount for Swiity Vending Machine
// Snap-fit / screw-mount case for the PN532 NFC reader
//
// PN532 V3 dimensions (from elechouse.com):
//   PCB: 42.7 x 40.4 x 4.0 mm (including components)
//   Mounting holes: 4x at corners, ~2.0 mm diameter
//   Mounting hole inset: ~2.5 mm from edges
//   Pin header: 8-pin, 2.54 mm pitch, along one short edge
//   Antenna: integrated, on the front face of the PCB

// --- Parameters ---

// PCB dimensions
pcb_w      = 42.7;   // width
pcb_d      = 40.4;   // depth
pcb_thick  = 4.0;    // component height (top side)
pcb_bot    = 1.2;    // component height (bottom side / solder joints)

// Mounting holes on the PCB
pcb_hole_d     = 2.0;
pcb_hole_inset = 2.5;  // from PCB edges to hole center

// Mount config
wall        = 2.0;
tolerance   = 0.3;
base_thick  = 2.0;
lip_height  = 2.5;   // lip to hold the PCB from the sides
post_h      = pcb_bot + 0.5; // standoff height to lift PCB off base
screw_d     = 2.2;   // M2 screw hole in standoffs
screw_head  = 4.0;   // M2 screw head clearance

// Faceplate window: the NFC antenna area should be unobstructed
// The antenna occupies roughly the center of the PCB
antenna_w   = 34.0;
antenna_d   = 32.0;

// Bracket mounting holes (to mount the whole thing to the vending machine)
bracket_hole_d = 4.0;  // M3 clearance
bracket_ear_w  = 10.0;
bracket_ear_h  = base_thick;

// Derived
inner_w = pcb_w + tolerance * 2;
inner_d = pcb_d + tolerance * 2;
outer_w = inner_w + wall * 2;
outer_d = inner_d + wall * 2;
total_h = base_thick + post_h + pcb_thick + lip_height;

// --- Modules ---

module standoff(x, y) {
    translate([wall + tolerance + x, wall + tolerance + y, base_thick]) {
        difference() {
            cylinder(h = post_h, d = 5.0, $fn = 24);
            translate([0, 0, -0.1])
                cylinder(h = post_h + 0.2, d = screw_d, $fn = 20);
        }
    }
}

module pcb_screw_positions() {
    // 4 corners of the PCB
    offsets = [
        [pcb_hole_inset, pcb_hole_inset],
        [pcb_w - pcb_hole_inset, pcb_hole_inset],
        [pcb_hole_inset, pcb_d - pcb_hole_inset],
        [pcb_w - pcb_hole_inset, pcb_d - pcb_hole_inset],
    ];
    for (p = offsets) children(p[0], p[1]);
}

module bracket_ears() {
    // Two ears on the sides for mounting to the machine
    for (side = [0, 1]) {
        mirror([side, 0, 0])
        translate([-bracket_ear_w, outer_d / 2 - bracket_ear_w / 2, 0]) {
            difference() {
                cube([bracket_ear_w, bracket_ear_w, bracket_ear_h]);
                translate([bracket_ear_w / 2, bracket_ear_w / 2, -0.1])
                    cylinder(h = bracket_ear_h + 0.2, d = bracket_hole_d, $fn = 24);
            }
        }
    }
}

// --- Base / tray ---
module base() {
    difference() {
        union() {
            // Main box
            cube([outer_w, outer_d, total_h]);

            // Mounting ears
            translate([0, 0, 0]) bracket_ears();
            translate([outer_w, 0, 0]) bracket_ears();
        }

        // Hollow out the inside (above the base plate)
        translate([wall, wall, base_thick])
            cube([inner_w, inner_d, total_h]);

        // Pin header cutout on one side
        translate([wall + tolerance + inner_w/2 - 12, -0.1, base_thick + post_h - 0.5])
            cube([24, wall + 0.2, pcb_thick + lip_height + 1]);
    }

    // Standoffs at PCB mounting hole positions
    pcb_screw_positions() standoff($children > 0 ? 0 : 0, 0);

    // Manual standoffs since children() trick doesn't work easily in OpenSCAD
    standoff(pcb_hole_inset, pcb_hole_inset);
    standoff(pcb_w - pcb_hole_inset, pcb_hole_inset);
    standoff(pcb_hole_inset, pcb_d - pcb_hole_inset);
    standoff(pcb_w - pcb_hole_inset, pcb_d - pcb_hole_inset);
}

// --- Faceplate (optional snap-on lid) ---
module faceplate() {
    difference() {
        // Outer plate
        cube([outer_w, outer_d, 2.0]);

        // Antenna window
        translate([outer_w/2 - antenna_w/2, outer_d/2 - antenna_d/2, -0.1])
            cube([antenna_w, antenna_d, 2.2]);

        // Screw holes to match PCB standoffs
        translate([wall + tolerance + pcb_hole_inset, wall + tolerance + pcb_hole_inset, -0.1])
            cylinder(h = 2.2, d = screw_d, $fn = 20);
        translate([wall + tolerance + pcb_w - pcb_hole_inset, wall + tolerance + pcb_hole_inset, -0.1])
            cylinder(h = 2.2, d = screw_d, $fn = 20);
        translate([wall + tolerance + pcb_hole_inset, wall + tolerance + pcb_d - pcb_hole_inset, -0.1])
            cylinder(h = 2.2, d = screw_d, $fn = 20);
        translate([wall + tolerance + pcb_w - pcb_hole_inset, wall + tolerance + pcb_d - pcb_hole_inset, -0.1])
            cylinder(h = 2.2, d = screw_d, $fn = 20);
    }

    // Label
    translate([outer_w/2, outer_d - 3, 2.0])
        linear_extrude(0.4)
            text("NFC", size = 6, halign = "center", font = "Liberation Sans:style=Bold");
}

// --- Render ---
// Uncomment one at a time for export:

// Base tray
base();

// Faceplate (shifted to the side for preview)
translate([outer_w + 10, 0, 0])
    faceplate();
