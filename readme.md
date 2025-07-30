# The BLE lock project

So there currently exist many smart lock vendors out there but all of them seem to leave something to desire.
I therefore wanted to take initiative and create an open source project that attempts to resolve many of these issues.

Don't be fooled by the name ble-lock it will be versatile enough for any credential eventually.


So what are the goals of this repo.
We will try to attempt the following

 * Nodes and the Cloud should be able to exchange all types of messages through the normal use of the system.
 * Users should be able to authenticate even if they are offline
 * Offline credentials should be securely distributed
 * It should be Fast to unlock a door (As few steps as possible)
 * If a credential/key is revoked there should be a mechanism to send a message through other users that spread
 * That message should only live as long as the key could be potentially used
 * Nodes that drift in time should be able to be "Repaired" securely

Stretch goals:
 * Nodes should mesh together over BLE
 * MAGIC


### Decisions made

#### TLV
> TLV is decided to be used as the underlying layer in all communication before and after encryption because it is
both flexible and lightweight.

### Keys
AES 256 keys are chosen for all crypto.
Symmetric crypto doesn't meet our requirements of security? Well if we want the protocol to be able to be used on stuff like NRF etc we need to make some compromises

What ids exist in the system then?
 - The systemID (a 4 Byte identifier indicating the building/mesh)
 - NodeID (a 2 byte identifier identifying the hw you speak to)
 - LockID (a 2 byte identifier identifying the actual entity your key should interact with)

There is the possibility for collisions in this scheme. 

The systemID is assigned by the cloud service together with a systemKey that is configured when the node is added
A NodeID is just generated at random on the node when it wakes up for the first time. Together with the Node admin key (Cloud Key)

Same with the LockID they get randomly created when a lock is configured onto the node.

What crypto keys do we have then

 - Each device gets 2 keys during installation.
   - Node Admin Key (Cloud key)
     - Used to sign commands, permissions etc examples will come later
   - Site/System key. This key is shared among all the nodes in the same building. This key is intended to be used to encrypt messages between nodes etc. For example if a node offers up it's more recent time that key is used for the communication.

 - Users/Credentials
   - A users "Credential" consists of 2 parts. One AES key used to sign commands and one encrypted BLOB using the Cloud key.

### Let's just dive in
How does the Admin authorize a user

We create an encrypted TLV object looking something like this:
> CredentialId: String[128]
CredentialType: 1Byte // 0x01 Mobile AES256 key
CredentialVersion: 1Byte // Number that increase for each signed message
SystemID: 4Byte
LockIDs: []2+1Byte // List of allowed doors the user can access includes all locks in the installation even if they are not attached to this node
// Last byte indicates the permissions you have on that lock.
// Bit 1 = Operate(Pulse)
// Bit 2 = Unlock (Leave open)
// Bit 3 = Lock   (Close door left open)
// Bit 8 = Allow remote (Allows to do it with forwarded messages either through the Mesh or IP)
ValidFrom: Epoch
ValidTo: Epoch
OperationKey: AES256KEY

On the server using the Node Admin Key
Then we provide this to the app as 2 parts
`{Authorization: "encryptedTlv", operationKey: "AES256KEY"}`
The user will also need to know what "System" and "Node" has the lock that he wants to operate. We assume that this is provided through other means than the "Credential"

When the user wants to issue a command to a door they will first look for ManufacturingData which will be advertising an unencrypted TLV
> Nonce: 8 Random Bytes
SystemID: 0xf001f001
LockID: 0x0001

The app will then create and encrypt the following using the OperationKey:
> Nonce: 8Bytes from advertisement
MessageType: 2Byte, PulseLock for example
Extra: Dynamic(For Pulse lock just the lock ID)

Then both of these encrypted TLV's get packaged in a 3 TLV
> RecieverNode: NodeID
Authorization: encryptedTlvFromCloud
Message: the cryptogram made by the app

Then this gets written to a Service on the node.
The response is then advertised back on another service and contains both a OperationResult and potentially some data to forward to the cloud. 

The message forwarding scheme. Whenever for example an audit trail is created a message is created with RecieverNode :0x0000 which is a special one for the cloud.
The messages have ID's and are stored in the node until the cloud generates an ACK message back that is transported similar to how the key is done but it's just a "AdminMessage" so no fancy key signing etc. Just a plain encryped message. All messages have messages ids and we just create a "Ack" message with no QOS we just send it with the first person who could possibly ack the message of the node and say that's fine. If the ack message does not reach the node the message will just be retransmitted and a new ack will be issued until someone can get the message across.

## Meshing
Very neat idea for example if one of the nodes in your mesh has internet the other devices can use that to transmit audit trails etc. Also this allows a user on the other side of a building to open the main entrance because they can just send out an open message on the mesh and it will reach

