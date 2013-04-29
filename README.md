#Results
<h3>without</h3>
<img src='http://micah-d-lamb.com/images/face/A-without.png'>
<h3>6 blurs, realistic diffusion</h3>
<img src='http://micah-d-lamb.com/images/face/A-6blurs-200worldScale.png'>
<h3>6 blurs, more diffused</h3>
<img src='http://micah-d-lamb.com/images/face/A-6blurs-350worldScale.png'>
<br><br>
<h3>without</h3>
<img src='http://micah-d-lamb.com/images/face/B-without.png'>
<h3>6 blurs, realistic diffusion</h3>
<img src='http://micah-d-lamb.com/images/face/B-6blurs-200worldScale.png'>
<h3>6 blurs, more diffused</h3>
<img src='http://micah-d-lamb.com/images/face/B-6blurs-350worldScale.png'>
<h3>specular at grazing angle</h3>
<img src='http://micah-d-lamb.com/images/face/spec.png'>

#Report
<a href='http://micah-d-lamb.com/skin.pdf'>report</a>

#Quick Overview of Important Files
<h3>Face.h</h3>
<p>
	Contains all opengl code specific to this project using the following classes:<br>
	<b>Face</b>: Constructs and draws the face.<br>
	<b>Face Scene</b>: Renders the face every frame and handles input.<br>
	<b>Blur</b>: Called by Face every render to blur the irradiance map.
</p>
<h3>convolve.vert/frag</h3>
<p>
	Used by Blur class to gaussian blur the irraidiance texture using the specified 6 gauss widths.
</p>
<h3>face.vert/frag</h3>
<p>
	Final pass Face shader which combines the blurred irradiance maps and specular brdf.
</p>