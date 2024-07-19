import asyncio
import json
import rti.asyncio
import rti.connextdds as dds
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
import uvicorn

# Import DDS data types from kitchen_idl module
from outdoor_idl import Survey, Parking, Water

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
        <div id="survey"></div>
        <div id="parking"></div>
        <div id="water"></div>
        <script>
            var ws = new WebSocket("ws://localhost:8000/ws");
            ws.onmessage = function(event) {
                var data = JSON.parse(event.data);
                if (data.type === "survey") {
                    document.getElementById("survey").innerText = data.message;
                } else if (data.type === "parking") {
                    document.getElementById("parking").innerText = data.message;
                } else if (data.type === "water") {
                    document.getElementById("water").innerText = data.message;
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
topic_survey = dds.Topic(participant, 'home/outdoor/survey', Survey)
reader_survey = dds.DataReader(participant.implicit_subscriber, topic_survey)

topic_parking = dds.Topic(participant, 'home/outdoor/parking', Parking)
reader_parking = dds.DataReader(participant.implicit_subscriber, topic_parking)

topic_water = dds.Topic(participant, 'home/outdoor/water', Water)
reader_water = dds.DataReader(participant.implicit_subscriber, topic_water)

# Async function to read and broadcast temperature data
async def read_and_broadcast_survey():
    async for data_survey in reader_survey.take_data_async():
        message_survey = f"Received Survey: {data_survey}"
        print(message_survey)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "survey", "message": message_survey}))

# Async function to read and broadcast humidity data
async def read_and_broadcast_parking():
    async for data_parking in reader_parking.take_data_async():
        message_parking = f"Received parking: {data_parking}"
        print(message_parking)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "parking", "message": message_parking}))

# Async function to read and broadcast smoke data
async def read_and_broadcast_water():
    async for data_water in reader_water.take_data_async():
        message_water = f"Received Water: {data_water}"
        print(message_water)
        # Send data to all connected WebSocket clients
        for conn in websocket_connections:
            await conn.send_text(json.dumps({"type": "water", "message": message_water}))

# Create background tasks to read and broadcast data for all types
@app.on_event("startup")
async def on_startup():
    asyncio.create_task(read_and_broadcast_survey())
    asyncio.create_task(read_and_broadcast_parking())
    asyncio.create_task(read_and_broadcast_water())

# Run the FastAPI server with Uvicorn
if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000)
