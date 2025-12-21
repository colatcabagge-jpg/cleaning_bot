from flask import Flask, render_template
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route("/")
def index():
    return render_template("index.html")

# ===== MQTT =====
def on_connect(client, userdata, flags, rc):
    client.subscribe("esp32/sensors")

def on_message(client, userdata, msg):
    if msg.topic == "esp32/sensors":
        data = json.loads(msg.payload.decode())
        socketio.emit("sensor_data", data)

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect("localhost", 1883, 60)
mqtt_client.loop_start()

# ===== RUN =====
if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=5000)
