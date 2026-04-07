// HC-SR04 Quad Sensor Mount for Swiity Vending Machine
// Holds 4 HC-SR04 ultrasonic sensors side by side
//
// HC-SR04 dimensions (from datasheet):
//   PCB: 45.0 x 20.0 x 15.0 mm
//   Transducer diameter: 16.0 mm
//   Transducer center-to-center: 26.0 mm
//   4-pin header pitch: 2.54 mm
//   Mounting holes: ~1.0 mm diameter at corners

// --- Parameters (tweak to fit your print / layout) ---

// Sensor dimensions
sensor_w     = 45.0;   // PCB width
sensor_h     = 20.0;   // PCB height (depth into mount)
sensor_thick = 1.6;    // PCB thickness
trans_dia    = 16.0;   // transducer cylinder diameter
trans_height = 12.0;   // transducer cylinder protrusion
trans_spacing = 26.0;  // center-to-center of the two transducers

// Mount config
num_sensors  = 4;
slot_gap     = 4.0;    // gap between sensor slots
wall         = 2.0;    // wall thickness around each slot
tolerance    = 0.4;    // print tolerance per side
base_thick   = 3.0;    // bottom plate thickness
lip_height   = 5.0;    // how far the side walls come up to hold the PCB
mount_hole_d = 4.0;    // M3 screw + clearance for mounting the bracket itself
mount_hole_inset = 5.0;

// Derived
slot_w = sensor_w + tolerance * 2;
slot_d = sensor_h + tolerance * 2;
total_w = num_sensors * (slot_w + wall) + wall + (num_sensors - 1) * slot_gap;
total_d = slot_d + wall * 2;
trans_hole_d = trans_dia + tolerance * 2;

module sensor_slot(x_offset) {
    translate([x_offset, wall, base_thick]) {
        // PCB slot (cut through the lip walls)
        cube([slot_w, slot_d, lip_height + 1]);
    }
    // Transducer holes through the base
    for (dx = [-trans_spacing/2, trans_spacing/2]) {
        translate([x_offset + slot_w/2 + dx, wall + slot_d/2, -0.1])
            cylinder(h = base_thick + 0.2, d = trans_hole_d, $fn = 48);
    }
    // Wire slot at the back for the pin header
    translate([x_offset + slot_w/2 - 8, wall + slot_d - 1, base_thick])
        cube([16, wall + 2, lip_height + 1]);
}

module mount_holes() {
    // Four corner mounting holes
    positions = [
        [mount_hole_inset, mount_hole_inset, -0.1],
        [total_w - mount_hole_inset, mount_hole_inset, -0.1],
        [mount_hole_inset, total_d - mount_hole_inset, -0.1],
        [total_w - mount_hole_inset, total_d - mount_hole_inset, -0.1],
    ];
    for (p = positions) {
        translate(p)
            cylinder(h = base_thick + 0.2, d = mount_hole_d, $fn = 32);
    }
}

// --- Main body ---
difference() {
    // Outer shell: base plate + lip walls
    cube([total_w, total_d, base_thick + lip_height]);

    // Cut the sensor slots
    for (i = [0 : num_sensors - 1]) {
        x = wall + i * (slot_w + wall + slot_gap);
        sensor_slot(x);
    }

    // Mounting holes
    mount_holes();
}

// --- Label ---
// Small embossed text on the front
translate([total_w / 2, 0.5, base_thick + lip_height - 0.5])
    rotate([90, 0, 0])
        linear_extrude(0.6)
            text("SWIITY", size = 5, halign = "center", font = "Liberation Sans:style=Bold");
