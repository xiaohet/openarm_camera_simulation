async function updateEpisodes()
{
    let response =
        await fetch("/episodes/count");

    let data =
        await response.json();

    document.getElementById("message").innerText =
        "Episodes: " + data.count;
}

async function updateJoints()
{
    let response =
        await fetch("/live/joints");

    let data =
        await response.json();

    let text = "";

    text += "Positions:\n";

    for(let i=0;i<data.position.length;i++)
    {
        text +=
            "Joint " +
            (i+1) +
            ": " +
            data.position[i] +
            "\n";
    }

    text += "\nVelocities:\n";

    for(let i=0;i<data.velocity.length;i++)
    {
        text +=
            "Joint " +
            (i+1) +
            ": " +
            data.velocity[i] +
            "\n";
    }

    text += "\nTorques:\n";

    for(let i=0;i<data.torque.length;i++)
    {
        text +=
            "Joint " +
            (i+1) +
            ": " +
            data.torque[i] +
            "\n";
    }

    document.getElementById(
        "joint_states"
    ).innerText = text;
}

async function updateEpisodeCount()
{
    let response =
        await fetch("/episodes/count");

    let data =
        await response.json();

    document.getElementById(
        "episode_count"
    ).innerText =
        "Episodes Recorded: " +
        data.count;
}

async function startRecording()
{
    await fetch(
        "/record/start",
        {
            method:"POST"
        }
    );

    updateRecordStatus();
}

async function stopRecording()
{
    await fetch(
        "/record/stop",
        {
            method:"POST"
        }
    );

    updateRecordStatus();
}

async function updateRecordStatus()
{
    let response =
        await fetch(
            "/record/status"
        );

    let data =
        await response.json();

    document.getElementById(
        "record_status"
    ).innerText =
        data.recording ?
        "Recording" :
        "Stopped";
}

function updateCameraFeeds()
{
    document.getElementById("left_cam").src =
        "/latest_frame/left_wrist?t=" +
        Date.now();

    document.getElementById("right_cam").src =
        "/latest_frame/right_wrist?t=" +
        Date.now();

    document.getElementById("ceiling_cam").src =
        "/latest_frame/ceiling?t=" +
        Date.now();

    document.getElementById("zed_cam").src =
        "/latest_frame/zed?t=" +
        Date.now();
}

setInterval(updateEpisodeCount, 1000);

updateRecordStatus();
setInterval(updateRecordStatus, 1000);

updateCameraFeeds();
setInterval(updateCameraFeeds, 1000);