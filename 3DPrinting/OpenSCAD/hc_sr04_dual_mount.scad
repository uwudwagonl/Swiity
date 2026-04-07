// HC-SR04 Dual Sensor Mount (vertical) for Swiity Vending Machine
// 2 sensors stacked vertically, 65mm apart (center-to-center)
// Print this twice for all 4 slots
//
// HC-SR04 dimensions:
//   PCB: 45.0 x 20.0 x 15.0 mm
//   Transducer diameter: 16.0 mm
//   Transducer center-to-center: 26.0 mm

// --- Parameters ---

// Sensor dimensions
sensor_w      = 45.0;
sensor_d      = 20.0;
trans_dia     = 16.0;
trans_spacing = 26.0;

// Mount config
sensor_gap    = 65.0;   // vertical center-to-center between the two sensors
wall          = 2.0;
tolerance     = 0.4;
base_thick    = 3.0;
lip_height    = 5.0;
mount_hole_d  = 4.0;    // M3 clearance
mount_hole_inset = 5.0;

// Derived
slot_w = sensor_w + tolerance * 2;
slot_d = sensor_d + tolerance * 2;
trans_hole_d = trans_dia + tolerance * 2;

// The two sensor slots are centered vertically with sensor_gap between them
// Total height of the bracket
slot_center_1 = wall + slot_d / 2;
slot_center_2 = slot_center_1 + sensor_gap;

total_w = slot_w + wall * 2;
total_h = slot_center_2 + slot_d / 2 + wall;
total_depth = base_thick + lip_height;

module sensor_slot(y_offset) {
    // PCB pocket
    translate([wall, y_offset, base_thick])
        cube([slot_w, slot_d, lip_height + 1]);

    // Transducer holes through the base plate
    for (dx = [-trans_spacing/2, trans_spacing/2]) {
        translate([wall + slot_w/2 + dx, y_offset + slot_d/2, -0.1])
            cylinder(h = base_thick + 0.2, d = trans_hole_d, $fn = 48);
    }

    // Wire cutout at the top of each slot for the pin header
    translate([wall + slot_w/2 - 8, y_offset + slot_d - 1, base_thick])
        cube([16, wall + 2, lip_height + 1]);
}

module mount_holes() {
    positions = [
        [mount_hole_inset, mount_hole_inset],
        [total_w - mount_hole_inset, mount_hole_inset],
        [mount_hole_inset, total_h - mount_hole_inset],
        [total_w - mount_hole_inset, total_h - mount_hole_inset],
        // Extra pair between the two sensors for rigidity
        [mount_hole_inset, (slot_center_1 + slot_center_2) / 2],
        [total_w - mount_hole_inset, (slot_center_1 + slot_center_2) / 2],
    ];
    for (p = positions) {
        translate([p[0], p[1], -0.1])
            cylinder(h = base_thick + 0.2, d = mount_hole_d, $fn = 32);
    }
}

// --- Main body ---
difference() {
    cube([total_w, total_h, total_depth]);

    // Sensor slot 1 (bottom)
    sensor_slot(slot_center_1 - slot_d / 2);

    // Sensor slot 2 (top, 65mm above)
    sensor_slot(slot_center_2 - slot_d / 2);

    // Mounting holes
    mount_holes();
}

// --- Reinforcement rib between the two slots ---
rib_y1 = slot_center_1 + slot_d / 2 + wall;
rib_y2 = slot_center_2 - slot_d / 2 - wall;
rib_height = rib_y2 - rib_y1;
if (rib_height > 2) {
    // Center rib
    translate([total_w / 2 - 1.5, rib_y1, 0])
        cube([3, rib_height, total_depth]);
}

// --- Label ---
translate([total_w / 2, wall / 2, total_depth - 0.5])
    rotate([90, 0, 0])
        linear_extrude(0.6)
            text("SWIITY", size = 4, halign = "center", font = "Liberation Sans:style=Bold");
