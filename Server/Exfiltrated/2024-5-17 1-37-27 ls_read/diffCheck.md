## ls_read "Demo":
`ls_read` is meant to exfiltrate the current working directory (we're working on a version that steals more sensitive data). <br />
To check that this code works, we ran it through a diff checker. Ideally, we shouldn't see any changes. <br>
<br />

Testing if binary `.exe` files are copied over correctly: <br />
<img src="https://i.imgur.com/KLAUrln.png" />
<br /> <br />
Testing if text files with ASCII versions of "illegal characters" are copied correctly: <br />
<img src="https://i.imgur.com/sasQ5ry.png" />
<br /> <br />
Testing if code files are copied over correctly: <br />
<img src="https://i.imgur.com/57eZzMy.png" />