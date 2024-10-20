<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebRTC to SIP Call</title>
</head>
<body>
    <h1>WebRTC to SIP Call</h1>
    <button id="startCall">Start Call</button>

    <!-- Ringtone audio element -->
    <audio id="ringtone" loop>
        <source src="har-har-shambhu-shiv-mahadev-63287.mp3" type="audio/mpeg">
       
    </audio>

    <script>
        const signalingServer = new WebSocket('ws://localhost:8080');
        let localPeerConnection;
        const configuration = {
            iceServers: [
                {  urls: 'stun:stun.l.google.com:19302' }
            ]
        };
        let iceCandidatesQueue = [];
        let ringtone = document.getElementById('ringtone'); // Ringtone element

        signalingServer.onopen = () => {
            console.log('WebSocket connection established.');
        };

        signalingServer.onmessage = async (message) => {
            const data = JSON.parse(message.data);
            console.log('Message received from signaling server:', data);

            if (data.type === 'answer') {
                try {
                    const remoteDesc = new RTCSessionDescription(data);
                    await localPeerConnection.setRemoteDescription(remoteDesc);
                    console.log('Received answer from SIP:', remoteDesc.sdp);

                    // Stop the ringtone when the SIP client answers
                    ringtone.pause();
                    ringtone.currentTime = 0;

                    // Add all queued ICE candidates
                    while (iceCandidatesQueue.length) {
                        const candidate = iceCandidatesQueue.shift();
                        await localPeerConnection.addIceCandidate(candidate);
                        console.log('Added queued ICE candidate:', candidate);
                    }
                } catch (error) {
                    console.error('Error setting remote description:', error);
                }
            } else if (data.type === 'iceCandidate') {
                try {
                    const candidate = new RTCIceCandidate(data.candidate);
                    if (localPeerConnection.remoteDescription) {
                        await localPeerConnection.addIceCandidate(candidate);
                        console.log('Added received ICE candidate:', candidate);
                    } else {
                        iceCandidatesQueue.push(candidate);
                        console.log('Queued ICE candidate:', candidate);
                    }
                } catch (error) {
                    console.error('Error adding received ICE candidate:', error);
                }
            }
        };

        signalingServer.onclose = () => {
            console.log('WebSocket connection closed.');
        };

        document.getElementById('startCall').onclick = async () => {
            localPeerConnection = new RTCPeerConnection(configuration);

            try {
                const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
                stream.getTracks().forEach(track => {
                    localPeerConnection.addTrack(track, stream);
                    console.log('Added audio track:', track);
                });

                localPeerConnection.onicecandidate = (event) => {
                    if (event.candidate) {
                        signalingServer.send(JSON.stringify({ type: 'iceCandidate', candidate: event.candidate }));
                        console.log('New ICE candidate sent:', event.candidate);
                    }
                };

                // Create the WebRTC offer
                const offer = await localPeerConnection.createOffer();
                await localPeerConnection.setLocalDescription(offer);
                signalingServer.send(JSON.stringify({ type: 'offer', sdp: offer.sdp }));
                console.log('SDP offer sent:', offer.sdp);

                // Start playing the ringtone after sending the offer
                ringtone.play().catch(error => {
                    console.error('Error playing ringtone:', error);
                });
            } catch (error) {
                console.error('Error starting call:', error);
            }
        };
    </script>
</body>
</html>
