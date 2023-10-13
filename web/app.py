from flask import Flask, render_template, request, jsonify
import paho.mqtt.client as paho
from paho import mqtt
from flask_socketio import SocketIO
import json

CLIENT_USERNAME = "web-client"
CLIENT_PW = "INF2004_team18"
HOST = "191331b7729648beb5d359a7925e03c4.s1.eu.hivemq.cloud"
latest_temperature = ""

app = Flask(__name__, static_folder='./static',
            template_folder="./templates")

# Don't forget to handle CORS if needed
socketio = SocketIO(app, cors_allowed_origins="*")


@app.route("/", methods=['GET', 'POST'])
def index():
    return render_template("index.html")


@app.route("/publish", methods=['GET', 'POST'])
def publish():
    client.publish('pico_w/recv', payload="hello from web", qos=1)
    return render_template("index.html")


@app.route("/send_control", methods=['POST'])
def send_control():
    data = request.get_json()
    direction = data.get('direction')

    # Publish the control data to the Pico
    client.publish('pico_w/recv', payload=direction, qos=1)
    return jsonify({"message": "Control data sent successfully"})


def on_message(client, userdata, message):
    global latest_temperature
    if message.topic == "pico_w/temperature":
        latest_temperature = message.payload.decode()
        socketio.emit('mqtt_temperature ', {'data': message.payload.decode()})


@app.route("/get_temperature", methods=['GET'])
def get_temperature():
    return jsonify(latest_temperature=latest_temperature)


if __name__ == "__main__":
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv31)
    # enable TLS for secure connection
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
    client.username_pw_set(CLIENT_USERNAME, CLIENT_PW)
    client.connect(HOST, 8883)

    client.subscribe("pico_w/send", qos=1)
    client.subscribe("pico_w/temperature", qos=1)

    # client.on_publish = on_publish
    client.on_message = on_message
    client.loop_start()
    socketio.run(app, debug=True)
