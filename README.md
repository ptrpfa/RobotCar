### INF2004 Embedded Systems Team 18: Robot Car

---
#### Setup
HiveMQ require that SNI be enabled on the client. This code uses a patched version of `mqtt.c` to support SNI. 

To do so, in your expansion of `pico-sdk`, run

```
cd lib/lwip
git apply path/to/picow/mqtt-sni.patch
```
**[OPTIONAL]**
Install the MQTT CLI **[Here](https://hivemq.github.io/mqtt-cli/docs/installation/)**.

---
#### Starting the Flask webserver

1. Change your current directory to the `web/` folder:
    ```
    cd web
    ```
2. Create a Python virtualenv on your local environment:
    ```
    python3 -m venv .venv
    ```

3. Activate your created virtualenv:
    ```
    source .venv/bin/activate
    ```

4. Download the application's required dependencies and libaries:
    ```
    pip3 install -r requirements.txt
    ```
5. Start the webserver
	```
	python app.py
	``` 
6. (Temp) Navigate to http://127.0.0.1:5000, the `publish` link will publish a message to the topic `pico_w/recv`  

#### Building Pico W (temp)

1. Configure `WIFI_SSID` and `WIFI_PASSWORD` cmake environment variables
2. Open up the `picow` folder in Pico VS Code and build the `mqtt` target in cmake
3. Download the `mqtt.uf2` firmware into Pico w 
4. Open the serial monitor,   the Pico W should connect to the mqtt broker, and publish to `pico_w/send` consistently. At the same time, it should be subscribed to `pico_w/recv`
5. Published messages from Pico W should appear in the Flask webserver terminal, and published messages from the webserver should appear in the serial monitor