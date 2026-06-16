from flask import Flask
from flask import jsonify
from flask import send_file
from flask import render_template
import random
import json
import os

app = Flask(__name__)

EPISODE_DIR = "."

recording = False

# get list of episodes
@app.route("/episodes")
def list_episodes():

    episodes = []

    for file in os.listdir(EPISODE_DIR):

        if file.endswith(".h5"):
            episodes.append(file)

    return episodes

# get metadata of an episode
@app.route(
    "/episodes/<episode>/metadata")
def metadata(episode):

    path = (
        f"{episode}.json"
    )

    with open(path) as f:
        data = json.load(f)

    return jsonify(data)

# download data of an episode
@app.route(
    "/episodes/<episode>/download")
def download_episode(
        episode):

    return send_file(
        f"{episode}.h5",
        as_attachment=True)

# main dashboard
@app.route("/")
def dashboard():
    return render_template("dashboard.html")

# dashboard: display joint states
@app.route("/live/joints")
def live_joints():

    return jsonify({
        "position": [
            round(random.uniform(-1.0,1.0),3)
            for _ in range(6)
        ],
        "velocity": [
            round(random.uniform(-0.5,0.5),3)
            for _ in range(6)
        ],
        "torque": [
            round(random.uniform(-2.0,2.0),3)
            for _ in range(6)
        ]
    })

# dashboard: display episode count
@app.route("/episodes/count")
def episode_count():

    count = 0

    for file in os.listdir("."):
        if file.endswith(".h5"):
            count += 1

    return jsonify({
        "count": count
    })

# dashboard: start recording button
@app.route(
    "/record/start",
    methods=["POST"]
)
def start_recording():

    global recording

    recording = True

    return jsonify({
        "status":"recording"
    })

# dashboard: stop recording button
@app.route(
    "/record/stop",
    methods=["POST"]
)
def stop_recording():

    global recording

    recording = False

    return jsonify({
        "status":"stopped"
    })

# dashboard: showing current recording state
@app.route("/record/status")
def record_status():

    return jsonify({
        "recording": recording
    })

# dashboard: camera preview
@app.route("/latest_frame/<camera_name>")
def latest_frame(camera_name):

    camera_dir = (
        f"build/episode_0001_images/{camera_name}"
    )

    print("Looking for:", os.path.abspath(camera_dir))

    if not os.path.exists(camera_dir):
        return "Camera folder not found", 404

    files = [
        f for f in os.listdir(camera_dir)
        if f.endswith(".png")
    ]

    if not files:
        return "No frames", 404

    latest = sorted(files)[-1]

    return send_file(
        os.path.join(camera_dir, latest),
        mimetype="image/png"
    )

# run local host
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)

