#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QCheckBox>
#include <QHBoxLayout>

#include <QUdpSocket>
#include <QKeyEvent>
#include <QWidget>
#include <QVector>
#include <QHostInfo>
#include <QHash>
#include <QPair>

class TextEdit : public QTextEdit {
	Q_OBJECT
public:
	TextEdit();

signals:
	void returnPressed();

private:
	int shiftPressed;
	bool isShiftPressed();
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
};

class Peer {
public:
	Peer();
	Peer(QHostAddress h, quint16 p);
	bool isEqual(Peer p);
	bool operator==(const Peer p);
	QString toString();

	QHostAddress host;
	quint16 port;
};

class PrivateMessage : public QWidget {
	Q_OBJECT
public:
	PrivateMessage();
	PrivateMessage(QString d);
private:
	TextEdit *msgText;
	QString destination;

signals:
	void closeWindow();
	void sendPM(QVariantMap);

public slots:
	void gotReturn();
};

class Files {
public:
	Files();
	QString filename;
	QByteArray blocklist;
	QByteArray blocklistHash;
	qint64 filesize;
};

class DownloadFile {
public:
	DownloadFile();
	QString targetNode;
	Files *file;
	QFile *writeFile;
	qint64 blocksDownloaded;
	Peer dest;
	QVariantMap *msg;
    bool isDownload;

	QTimer *retransmit;
};

class FileSharing : public QFileDialog {
	Q_OBJECT
public:
	FileSharing();
	QVector<Files> files;
    Files* getFile(QString fileName);
signals:
	void shareFiles(FileSharing*);
public slots:
	void gotFilesSelected(QStringList fileList);
};

class FingerTableItem {
public:
    FingerTableItem();
    int intervalStart;
    int intervalEnd;
    QString originID;
};


class FingerTable {
public:
    QString oneBehind;
    int curHash;
    QVector<FingerTableItem*> items;
    FingerTable();
    FingerTable(int nSpots, QString originID);
    // Debug only to print the finger table 
    void printFingerTable();
    // to get the hash function
	int getHash(int nSpots, QString originId); 
	// to add a Node 
	void addNode(int nSpots, QString originID); 
    // based on a hash get the corresponding string
    QString getPeerFromHash(int hash);
    void updateBehindHash(int nSpots, QString newID);
    // get the distance from the current to the destination, wrapping ish!
    int getDistance(int nSpots, int dest, int cur);
};

class NetSocket : public QUdpSocket {
	Q_OBJECT

public:
	NetSocket();
	QString getOriginID();
	quint32 getSeqNo();
	quint16 getThisPort();
	Peer getThisPeer();
	void incSeqNo();
	bool getNF();
	bool isDownloading();
	QByteArray getDfileBlockReq();
	QString getTargetNode();
	// files that i've tried to upload to the DHT. 
	QMap<QString, QPair<QByteArray, QString> > *uploadedFiles;



	QComboBox *originList;

	// Bind local peerster to a socket
	bool bind();
	// Archive msg, update status
	void processMsg(QVariantMap *msg);
	// Send msg to peer p and start timer
	void monger(QVariantMap *msg, Peer *p);
	// Pick a random peer that is not the sender, returning NULL
	// if there are no peers other than the one given
	Peer* pickPeer(Peer sender);
	// Returns true if msg has "Origin" and "SeqNo" fields
	// and is the wanted SeqNo. Adds msg origin to archive and status
	// if new
	bool isMsgOrRouteOrDHT(QVariantMap msg, Peer *senderPeer);
	// Returns true if msg has "Dest", "Origin", and "HopLimit" fields,
	// in addition to one set required for either PM, BlockReq, or BlockReply
	bool isP2P(QVariantMap msg);
	// Returns true if msg has "Origin", "Search", and "Budget" fields
	bool isSearchReq(QVariantMap msg);
	// Send status to peer p
	void sendStatus(Peer *p);
	// Turn off timer and (1) send a message senderPeer needs,
	// (2) send a status to senderPeer to ask for a message,
	// or (3) flip a coin to decide whether to continue
	// rumormongering.
	void processStatus(QVariantMap msg, Peer senderPeer);
	// Add msgOrigin to archive and status
	void archiveNew(QString msgOrigin);
	// If it doesn't already exist, add a peer with
	// given attributes to peerList
	void learnPeer(QHostAddress sender, quint16 senderPort);
	// Send message msg to peer p
	void sendMsg(QVariantMap *msg, Peer p);
	// Convert arg to a peer and add it to peerList if valid
	void argToPeer(QString arg);
	// Add arguments to routingTable
	void addToRT(QString origin, Peer *p);
	// Send a route rumor message to the given peer
	void sendRoute(Peer p);
	// Send msg to "Dest" peer according to routingTable
	void forwardP2P(QVariantMap msg);
	// Broadcast route (own if msg == NULL) to all peers
	// excluding senderPeer
	void broadcast(QVariantMap *msg, Peer *senderPeer);
	// Return blocklist metafile or requested block of data
	// by searching for blockReq among archived files - 
	// either the blocklistHash or a 20-byte chunk of the
	// blocklist
	QByteArray findBlock(QByteArray blockReq);
	// Update file download appropriately, sending next request, restarting
	// timer, and deleting file download as necessary
	void processBlockReply(QByteArray data);
	// Search for search request string among file names in
	// fileArchive; send search reply if found
	void processSearchReq(QVariantMap msg, Peer p);
	// Send message to peers, dividing the budget up by the
	// budget currently indicated in msg
	void sendByBudget(QVariantMap msg);
	// Process request to join DHT
	void processJoinReq(QVariantMap msg);
	// Add msgOrigin to dhtStatus
	void archiveNewDHT(QString msgOrigin);
	// Update dhtStatus to reflect new (higher) seqno and join state
	void updateDhtStatus(QVariantMap *msg);
	// Add to finger table
	void addToFingerTable(QString origin);


    // new stuff
    // is TransferRequest
    bool isTransferRequest(QVariantMap msg);
    // foundTransferRequest
    void doTransferRequest(QVariantMap msg);

	// reply to transfer request!
    void replyToTransferRequest(QVariantMap msg); 

    // send it to corresponding node in fingerTable
    void sendThroughFingerTable(QVariantMap *msg);
    bool isMyDHTRequest(int desiredLoc);
    QMap<QString, Files> *dhtArchive;
    QString removePrefix(QString withPrefix);
    void copyFile(QVariantMap msg);
    void tranferToAddedNode();
    void deleteDHTFilesFromNode(FileSharing *toDelete);



private:
	quint16 myPortMin, myPortMax, thisPort;
	Peer *thisPeer;
	QString originID;
	quint32 seqNo, dhtSeqNo;
	quint32 nSpots;
	quint32 myDHTHash;
	FingerTable *fingerTable;
	// List of originIDs with lowest sequence number not seen
	QVariantMap *status;
	// List of originIDs with lowest sequence number not seen
	QMap<QString, QPair<quint32, bool> > *dhtStatus;
	// Archive of all messages
	QMap<QString, QMap<quint32, QVariant> > *archive;
	// List of all peers (excluding self)
	QVector<Peer> peerList;
	// Temporary holder for peers while performing host lookup
	QMap<QString, quint16> *hostPorts;
	// List of timers for regular timeouts
	QVector<QTimer*> *timerList;
	// Entropy timer
	QTimer *entropyTimer;
	// Hop list
	QHash<QString, Peer*> *routingTable;
	// Route rumor timer
	QTimer *routeTimer;
	// Flag for whether noforward command link option is specified
	bool noForward;
	// Archive of Files on this peer
	QMap<QString, Files> *fileArchive;
	// Whether there is a file being downloaded
	bool downloading;
	// Information on the file being downloaded
	DownloadFile *dfile;
	// Whether the user wants to join the DHT
	bool joinDHT;
	// Whether the user has joined the DHT
	bool hasJoinedDHT;

signals:
	void joinedDHT();
	void leftDHT();

public slots:
	void gotTimeout();
	void gotEntropyTimeout();
	void gotRouteTimeout();
	void lookedUp(QHostInfo hostInfo);
	void resetOL();
	void gotSendPM(QVariantMap msg);
	void gotShareFiles(FileSharing *share);
    void gotReqToDownload(QPair<QString, QPair<QByteArray, QString> > pair, bool isDownload);
	void gotRetransmit();
	void gotStartSearchFor(QPair<QString, quint32> pair);
	void changedDHTPreference(int state);
};

class ChatDialog : public QDialog {
	Q_OBJECT

public:
	ChatDialog();

public slots:
	void gotReturnPressed();
	void gotPortInput();
	void displayText(QString sender, QString text);
	void readMsg();
	void processMsgOrRouteOrDHT(QVariantMap msg, Peer *senderPeer);
	void newPrivateMsg(QString origin);
	void shareFile();
	void gotDownloadReq();
	void gotDownloadReqFromSearch(QListWidgetItem *item);
	void gotSearchInput();
	void gotJoinedDHT();
	void gotLeftDHT();

	// Process search reply, adding information to
	// searchReplyArchive and displaying for user
	void processSearchRep(QVariantMap msg);

	// Reset the timer and increase the budget
	void increaseBudget();

signals:
    void reqToDownload(QPair<QString, QPair<QByteArray, QString> >, bool);
	void startSearchFor(QPair<QString, quint32>);

private:
	NetSocket *sock;
	QTextEdit *textview;
	QLineEdit *portInput;
	TextEdit *textline;
	QLabel *pmLabel, *downloadLabel, *dhtLabel;
	QPushButton *fileShare, *downloadFile, *searchButton;
	QLineEdit *targetNode, *hexBlock, *searchField;
	QListWidget *searchResults;
	QCheckBox *joinDHTBox;
	QHBoxLayout *dht;

	// Search request currently awaiting replies
	QString searchRequest;
	QMap<QString, QPair<QByteArray, QString> > *searchReplyArchive;
	quint32 budget;

	// Timer to resend search request
	QTimer *searchTimer;

};

#endif // PEERSTER_MAIN_HH
