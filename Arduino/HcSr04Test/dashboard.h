const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
        :root {
            --bg-color: #1a1a2e;
            --container-bg: rgba(255, 255, 255, 0.05);
            --primary-text: #e2e8f0;
            --accent-color: #4f46e5;
            --accent-hover: #4338ca;
            --danger-color: #ef4444;
            --success-color: #10b981;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: var(--bg-color);
            color: var(--primary-text);
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }

        .dashboard {
            background: var(--container-bg);
            border-radius: 16px;
            padding: 2rem;
            width: 90%;
            max-width: 400px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.1);
            text-align: center;
        }

        h1 {
            margin-top: 0;
            font-size: 1.8rem;
            font-weight: 600;
            color: white;
            margin-bottom: 1.5rem;
        }

        .sensor-card {
            background: rgba(0, 0, 0, 0.2);
            padding: 1.5rem;
            border-radius: 12px;
            margin-bottom: 1.5rem;
        }

        .distance-value {
            font-size: 3rem;
            font-weight: 700;
            color: #60a5fa;
            margin: 0.5rem 0;
            transition: color 0.3s;
        }

        .distance-unit {
            font-size: 1.2rem;
            color: #94a3b8;
        }

        .controls {
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 1rem;
            padding: 1.5rem;
            background: rgba(0, 0, 0, 0.2);
            border-radius: 12px;
        }

        .btn {
            background: var(--accent-color);
            color: white;
            border: none;
            padding: 0.8rem 1.5rem;
            border-radius: 8px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }

        .btn:hover {
            background: var(--accent-hover);
            transform: translateY(-2px);
        }

        .btn:active {
            transform: translateY(0);
        }

        .status-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: var(--danger-color);
            display: inline-block;
            transition: background 0.3s;
        }

        .status-dot.active {
            background: var(--success-color);
            box-shadow: 0 0 10px var(--success-color);
        }
    </style>
</head>
<body>

    <div class="dashboard">
        <h1>ESP32 Control Hub</h1>
        
        <div class="sensor-card">
            <div style="color: #94a3b8; font-size: 0.9rem; text-transform: uppercase; letter-spacing: 1px;">Ultrasonic Sensor</div>
            <div class="distance-value" id="distance">-- <span class="distance-unit">cm</span></div>
        </div>

        <div class="controls">
            <span style="font-weight: 500;">Onboard LED</span>
            <div class="status-dot" id="led-indicator"></div>
            <button class="btn" onclick="toggleLED()">Toggle</button>
        </div>
    </div>

    <script>
        function updateDistance() {
            fetch('/distance')
                .then(response => response.json())
                .then(data => {
                    const distElement = document.getElementById('distance');
                    distElement.innerHTML = `${data.distance.toFixed(1)} <span class="distance-unit">cm</span>`;
                    
                    // Visual feedback based on distance
                    if (data.distance < 10) {
                        distElement.style.color = '#ef4444'; // Red if too close
                    } else if (data.distance < 30) {
                        distElement.style.color = '#fbbf24'; // Yellow if medium
                    } else {
                        distElement.style.color = '#60a5fa'; // Blue if far
                    }
                })
                .catch(error => console.error('Error fetching distance:', error));
        }

        function checkLedState() {
            fetch('/state')
                .then(response => response.json())
                .then(data => {
                    updateLedIndicator(data.state);
                })
                .catch(error => console.error('Error fetching LED state:', error));
        }

        function toggleLED() {
            fetch('/toggle', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    updateLedIndicator(data.state);
                })
                .catch(error => console.error('Error toggling LED:', error));
        }

        function updateLedIndicator(isOn) {
            const indicator = document.getElementById('led-indicator');
            if (isOn) {
                indicator.classList.add('active');
            } else {
                indicator.classList.remove('active');
            }
        }

        // Setup polling
        setInterval(updateDistance, 500);
        
        // Initial state check
        checkLedState();
    </script>
</body>
</html>
)rawliteral";
