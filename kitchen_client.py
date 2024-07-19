import asyncio
import json
import rti.asyncio
import rti.connextdds as dds
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
import uvicorn

# Import DDS data types from kitchen_idl module
from kitchen_idl import Temp, Humi, Smoke

# FastAPI app instance
app = FastAPI()

# Store active WebSocket connections
websocket_connections = set()

# HTML content for testing WebSocket communication in a browser
html_content = """
<!DOCTYPE html>
<html>
    <head>
        <title>WebSocket DDS Example</title>
    </head>
    <body>
        <h1>WebSocket DDS Data</h1>
        <div id="temperature"></div>
        
        <div id="humidity"></div>
        <div id="smoke"></div>
        <script>
            var ws = new WebSocket("ws://localhost:8000/ws");
            ws.onmessage = function(event) {
                var data = JSON.parse(event.data);
                if (data.type === "temperature") {
                    document.getElementById("temperature").innerText = data.message;
                } else if (data.type === "humidity") {
                    document.getElementById("humidity").innerText = data.message;
                } else if (data.type === "smoke") {
                    document.getElementById("smoke").innerText = data.message;
                }
            };
        </script>
    </body>
</html>
"""

@app.get("/")
def get():
    # Serve a simple HTML page to test WebSocket connection
    return HTMLResponse(html_content)

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    # Accept the WebSocket connection
    await websocket.accept()
    websocket_connections.add(websocket)
    try:
        while True:
            # Keep the connection alive
            await websocket.receive_text()
    except WebSocketDisconnect:
        websocket_connections.remove(websocket)

# DomainParticipant and DDS setup for temperature, humidity, and smoke
participant = dds.DomainParticipant(domain_id=0)
topic_temp = dds.Topic(participant, 'home/kitchen/temp', Temp)
reader_temp = dds.DataReader(participant.implicit_subscriber, topic_temp)

topic_humi = dds.Topic(participant, 'home/kitchen/humi', Humi)
reader_humi = dds.DataReader(participant.implicit_subscriber, topic_humi)

topic_smoke = dds.Topic(participant, 'home/kitchen/smoke', Smoke)
reader_smoke = dds.DataReader(participant.implicit_subscriber, topic_smoke)

# Async function to read and broadcast temperature data
async def read_and_broadcast_temp():
    async for data_temp in reader_temp.take_data_async():
        message_temp = f"Received Temperature: {data_temp}"
        print(message_temp)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "temperature", "message": message_temp}))

# Async function to read and broadcast humidity data
async def read_and_broadcast_humi():
    async for data_humi in reader_humi.take_data_async():
        message_humi = f"Received Humidity: {data_humi}"
        print(message_humi)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "humidity", "message": message_humi}))

# Async function to read and broadcast smoke data
async def read_and_broadcast_smoke():
    async for data_smoke in reader_smoke.take_data_async():
        message_smoke = f"Received Smoke: {data_smoke}"
        print(message_smoke)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "smoke", "message": message_smoke}))

# Create background tasks to read and broadcast data for all types
@app.on_event("startup")
async def on_startup():
    asyncio.create_task(read_and_broadcast_temp())
    asyncio.create_task(read_and_broadcast_humi())
    asyncio.create_task(read_and_broadcast_smoke())

# Run the FastAPI server with Uvicorn
if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000)
