#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QChar>
#include <QSizePolicy>
#include <QVariantMap>
#include <QHostAddress>
#include <QIODevice>
#include <QTimer>
#include <QtCrypto>
#include <QFile>
#include <QRegExp>

#include "main.hh"

// Message field identifiers
const QString CHATTEXT = QString("ChatText");
const QString SEQNO = QString("SeqNo");
const QString WANT = QString("Want");
const QString ORIGIN = QString("Origin");
const QString DEST = QString("Dest");
const QString HOPLIMIT = QString("HopLimit");
const QString LASTIP = QString("LastIP");
const QString LASTPORT = QString("LastPort");
const QString BLOCKREQ = QString("BlockRequest");
const QString BLOCKREPLY = QString("BlockReply");
const QString DATA = QString("Data");
const QString SEARCH = QString("Search");
const QString BUDGET = QString("Budget");
const QString SEARCHREP = QString("SearchReply");
const QString MATCHNAMES = QString("MatchNames");
const QString MATCHIDS = QString("MatchIDs");
const QString JOINDHT = QString("JoinDHT");

// Default hop limit
const quint32 DEFLIM = 10;
// Default max number of bytes for chunking files
const qint64 MAXBYTES = 8000;
// Default budget
const quint32 DEFBUDGET = 2;

// TEXTEDIT FUNCTIONS ------------------------------------------------

TextEdit::TextEdit() {
	this->shiftPressed = 0;
}

bool TextEdit::isShiftPressed() {
	return (this->shiftPressed > 0);
}

void TextEdit::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Shift) {
		this->shiftPressed += 1;
		QTextEdit::keyPressEvent(event);
	} else if (event->key() == Qt::Key_Return && !this->isShiftPressed()) {
		emit returnPressed();
	} else {
		QTextEdit::keyPressEvent(event);
	}
}

void TextEdit::keyReleaseEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Shift && this->isShiftPressed()) {
		this->shiftPressed -= 1;
	}
	QTextEdit::keyReleaseEvent(event);
}

// DIALOG FUNCTIONS ------------------------------------------------

ChatDialog::ChatDialog() {
	// Create a UDP network socket
	sock = new NetSocket();
	if (!sock->bind()) {
		exit(1);
	}
	connect(sock, SIGNAL(readyRead()), this, SLOT(readMsg()));

	// Read-only text box where we display messages from everyone.
	// This widget expands both horizontally and vertically.
	textview = new QTextEdit(this);
	textview->setReadOnly(true);

	// Small text-entry box where the user can enter messages.
	// This widget expands only horizontally,
	// leaving extra vertical space for the textview widget.
	textline = new TextEdit();
	textline->setFixedHeight(80);
	connect(textline, SIGNAL(returnPressed()),
		this, SLOT(gotReturnPressed()));

	// Small text-entry box where the user can enter new ports to add.
	portInput = new QLineEdit(this);
	portInput->setPlaceholderText(QString("Add Port"));
	connect(portInput, SIGNAL(returnPressed()), this, SLOT(gotPortInput()));

	// List of known ports for starting private messages.
	pmLabel = new QLabel(this);
	pmLabel->setText("Start private message with");
	sock->originList = new QComboBox(this);
	connect(sock->originList, SIGNAL(activated(QString)),
		sock, SLOT(resetOL()));
	connect(sock->originList, SIGNAL(activated(QString)),
		this, SLOT(newPrivateMsg(QString)));

	// Button to share a file with a peer
	fileShare = new QPushButton(QString("Share File..."), this);
	connect(fileShare, SIGNAL(clicked()), this, SLOT(shareFile()));

	// Input fields to download a file from a peer
	downloadLabel = new QLabel(this);
	downloadLabel->setText("Download file");
	targetNode = new QLineEdit(this);
	targetNode->setPlaceholderText(QString("Target Node ID"));
	hexBlock = new QLineEdit(this);
	hexBlock->setPlaceholderText(QString("Metafile Hash"));
	downloadFile = new QPushButton(QString("Download File"), this);
	connect(downloadFile, SIGNAL(clicked()), this, SLOT(gotDownloadReq()));
	connect(this, SIGNAL(reqToDownload(QPair<QString, QPair<QByteArray, QString> >)),
		sock, SLOT(gotReqToDownload(QPair<QString, QPair<QByteArray, QString> >)));

	// Search field
	searchField = new QLineEdit(this);
	searchField->setPlaceholderText("Search for strings");
	searchButton = new QPushButton("Search");
	connect(searchButton, SIGNAL(clicked()), this, SLOT(gotSearchInput()));
	connect(this, SIGNAL(startSearchFor(QPair<QString, quint32>)),
		sock, SLOT(gotStartSearchFor(QPair<QString, quint32>)));
	// Search results list
	searchResults = new QListWidget(this);
	connect(searchResults, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
		this, SLOT(gotDownloadReqFromSearch(QListWidgetItem*)));
	// Initialize searchTimer
	searchTimer = new QTimer(this);
	connect(searchTimer, SIGNAL(timeout()),
		this, SLOT(increaseBudget()));

	// Initialize searchReplyArchive
	searchReplyArchive = new QMap<QString, QPair<QByteArray, QString> >();

	// DHT fields
	dhtLabel = new QLabel(this);
	dhtLabel->setText("Status: Not in DHT");
	connect(sock, SIGNAL(joinedDHT()), this, SLOT(gotJoinedDHT()));
	connect(sock, SIGNAL(joinedDHT()), sock, SLOT(gotJoinedDHT())); // TODO(rachel): remove this when isEmptyDHT is implemented
	connect(sock, SIGNAL(leftDHT()), this, SLOT(gotLeftDHT()));
	joinDHTBox = new QCheckBox(QString("Join DHT When Available"), this);
	connect(joinDHTBox, SIGNAL(stateChanged(int)),
		sock, SLOT(changedDHTPreference(int)));

	// Lay out the widgets to appear in the main window.
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(portInput);
	layout->addWidget(textview);
	layout->addWidget(textline);
	layout->addWidget(pmLabel);
	layout->addWidget(sock->originList);
	layout->addWidget(fileShare);
	layout->addWidget(downloadLabel);
	QHBoxLayout *download = new QHBoxLayout();
	download->addWidget(targetNode);
	download->addWidget(hexBlock);
	download->addWidget(downloadFile);
	layout->addLayout(download);
	QHBoxLayout *search = new QHBoxLayout();
	search->addWidget(searchField);
	search->addWidget(searchButton);
	layout->addLayout(search);
	layout->addWidget(searchResults);
	dht = new QHBoxLayout();
	dht->addWidget(dhtLabel);
	dht->addWidget(joinDHTBox);
	dht->setAlignment(joinDHTBox, Qt::AlignRight);
	layout->addLayout(dht);

	setLayout(layout);

	// Automatically set focus to textline
	textline->setFocus();

	setWindowTitle(QString("Peerster (").
		       append(sock->getOriginID()).
		       append(QString(")")));
}

void ChatDialog::gotReturnPressed() {
	QString text = textline->toPlainText();

	QVariantMap *msg = new QVariantMap();
	msg->insert(CHATTEXT, QVariant(text));
	msg->insert(ORIGIN, sock->getOriginID());
	msg->insert(SEQNO, sock->getSeqNo());
	sock->incSeqNo();

	// Archive message, update status
	sock->processMsg(msg);

	// Monger
	sock->monger(msg, sock->pickPeer(sock->getThisPeer()));

	// Display text
	displayText(QString("Me"), text);

	// Clear the textline to get ready for the next input message.
	textline->clear();
}

void ChatDialog::gotPortInput() {
	sock->argToPeer(portInput->text());
	// Broadcast message to all peers
	Peer p = sock->getThisPeer();
	sock->broadcast(NULL, &p);
	portInput->clear();
}

void ChatDialog::gotDownloadReq() {
	qDebug() << "User request to download" << hexBlock->text()
		 << "from" << targetNode->text();
	QPair<QByteArray, QString> pair =
		qMakePair(QByteArray::fromHex(hexBlock->text().toLatin1()), targetNode->text());
	QPair<QString, QPair<QByteArray, QString> > fullPair =
		qMakePair(targetNode->text(), pair);
	targetNode->clear();
	hexBlock->clear();


	if (sock->isDownloading()) {
		qDebug() << "request denied: other download in progress";
	} else {
		emit reqToDownload(fullPair);
	}
}

void ChatDialog::gotDownloadReqFromSearch(QListWidgetItem *item) {
	qDebug() << sock->getOriginID() << "got user request to download"
		 << item->text();
	QString filename = item->text();
	if (searchReplyArchive->find(filename) == searchReplyArchive->end()) {
		qDebug() << "internal error finding file";
	} else {
		// Find filename in searchReplyArchive and send block request
		QPair<QByteArray, QString> pair = searchReplyArchive->value(filename);
		QPair<QString, QPair<QByteArray, QString> > fullPair =
			qMakePair(filename, pair);
		sock->gotReqToDownload(fullPair);
	}
}

void ChatDialog::gotSearchInput() {
	qDebug() << sock->getOriginID()
		 << "search input:" << searchField->text();

	budget = DEFBUDGET;
	QPair<QString, quint32> pair =
		qMakePair(searchField->text(), budget);
	emit startSearchFor(pair);
	searchRequest = searchField->text();
	searchField->clear();

	// Clear old search results when new search input entered
	searchResults->clear();
	searchReplyArchive->clear();

	// Start searchTimer
	searchTimer->start(1000);
}

void ChatDialog::gotJoinedDHT() {
	dhtLabel->setText("Status: Joined DHT");
	dht->removeWidget(joinDHTBox);
	delete joinDHTBox;
}

void ChatDialog::gotLeftDHT() {
	dhtLabel->setText("Status: Not in DHT");
}

void ChatDialog::increaseBudget() {
	searchTimer->stop();
	budget *= 2;
	if (searchReplyArchive->size() < 10 && budget <= 128) {
		qDebug() << sock->getOriginID()
			 << "increased budget to" << budget;
		QPair<QString, quint32> pair =
			qMakePair(searchRequest, budget);
		emit startSearchFor(pair);
		searchTimer->start(1000);
	}
}

void ChatDialog::displayText(QString sender, QString text) {
	textview->append(sender.append(QString(":\n > ")).append(text));
}

void ChatDialog::readMsg() {
	while (sock->hasPendingDatagrams()) {
		// Read incoming datagram
		QByteArray datagram;
		datagram.resize(sock->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		sock->readDatagram(datagram.data(), datagram.size(),
				   &sender, &senderPort);

		// Learn new peers
		sock->learnPeer(sender, senderPort);

		Peer *senderPeer = new Peer(sender, senderPort);

		QVariantMap msg;    
		QDataStream s(&datagram, QIODevice::ReadOnly);
		s >> msg;

		// Triage based on datagram type
		if (sock->isP2P(msg)) {
			QString msgDest = msg.value(DEST).toString();
			quint32 hopLim = msg.value(HOPLIMIT).toUInt();

			if (msgDest == sock->getOriginID()) {
				// This peerster is the destination

				if (msg.find(CHATTEXT) != msg.end()) {
					// Display PM
					displayText(msg.value(ORIGIN).toString().
						    append(QString(" (PM)")),
						    msg.value(CHATTEXT).toString());
				} else if (msg.find(BLOCKREQ) != msg.end()) {
					qDebug() << sock->getOriginID() << 
						"received block request from" <<
						msg.value(ORIGIN).toString();

					// A BlockRequest can be the hash of either a data block
					// or a blocklist metafile
					QByteArray blockReq = msg.value(BLOCKREQ).toByteArray();

					// Find block or blocklist metadata
					// from internal database
					QByteArray foundBlock = sock->findBlock(blockReq);
					if (!(foundBlock.isEmpty())) {
						// Send reply
						QVariantMap *rep = new QVariantMap();
						rep->insert(ORIGIN, sock->getOriginID());
						rep->insert(DEST, msg.value(ORIGIN).toString());
						rep->insert(BLOCKREPLY, blockReq);
						rep->insert(DATA, foundBlock);
						rep->insert(HOPLIMIT, DEFLIM);

						sock->sendMsg(rep, *senderPeer);
					}
				} else if (msg.find(BLOCKREPLY) != msg.end()) {
					qDebug() << sock->getOriginID() <<
						"received block reply from" <<
						msg.value(ORIGIN).toString();

					QByteArray blockReply = msg.value(BLOCKREPLY).
						toByteArray();
					QByteArray data = msg.value(DATA).toByteArray();

					// Check that this data is expected
					if (blockReply != sock->getDfileBlockReq() ||
					    msg.value(ORIGIN).toString() !=
					    sock->getTargetNode()) {
						qDebug() << sock->getOriginID() <<
							"received unrequested reply";
					} else {
						// Check that hash of data == blockReply
						QCA::init();
						if (!QCA::isSupported("sha1")) {
							qDebug() << "error: SHA-1 not supported";
							return;
						}
						QCA::Hash shaHash("sha1");
						shaHash.update(data);
						if (shaHash.final().toByteArray() == blockReply) {
							sock->processBlockReply(data);
						} else {
							qDebug() << sock->getOriginID()
								 << "hashes not equal";
						}
						// NOTE: discards message where hashes don't agree
					}
				} else {
					processSearchRep(msg);
				}
			} else if (!sock->getNF() && hopLim > 1) {
				// Forward if a forwarding peer
				qDebug() << sock->getOriginID() << "got P2P from " <<
					senderPeer->toString() << "destined for " <<
					msgDest;

				// Forward msg to appropriate peer
				sock->forwardP2P(msg);
			}
			// NOTE: Discards msg that has reached the end of its hop limit
		} else if (sock->isSearchReq(msg)) {
			// Search for string and send search reply if matches found
			sock->processSearchReq(msg, *senderPeer);

			// Redistribute search request if more budget left
			quint32 newBudget = msg.value(BUDGET).toUInt() - 1;
			if (newBudget > 0) {
				msg.remove(BUDGET);
				msg.insert(BUDGET, newBudget);
				sock->sendByBudget(msg);
			}
		} else if (sock->isMsgOrRouteOrDHT(msg, senderPeer)) {
			// Datagram is a message or route rumor

			// Display message
			if (msg.find(CHATTEXT) != msg.end()) {
				qDebug() << sock->getOriginID() << "got msg from " <<
					senderPeer->toString();

				displayText(msg.value(ORIGIN).toString(),
					    msg.value(CHATTEXT).toString());
			}
			qDebug() << sock->getOriginID() << "got route from " << 
				senderPeer->toString();
			
			// Process msg/route
			processMsgOrRouteOrDHT(msg, senderPeer);
		} else if (msg.find(WANT) != msg.end()) {
			// Datagram is a status
			sock->processStatus(msg, *senderPeer);
		} else {
			// Missing datagram fields or unwanted SeqNo
			sock->sendStatus(senderPeer);
		}
	}
}

void ChatDialog::processMsgOrRouteOrDHT(QVariantMap msg, Peer *senderPeer) {
	// Add to routingTable
	sock->addToRT(msg.value(ORIGIN).toString(), senderPeer);

	if (msg.find(JOINDHT) == msg.end()) {
		// If chat msg or route rumor

		// Archive message, update status
		sock->processMsg(&msg);
	
		// Add last public IP address to peerList
		QHostAddress h;
		if (msg.find(LASTIP) != msg.end() && 
		    msg.find(LASTPORT) != msg.end()) {
			h = QHostAddress(msg[LASTIP].toUInt());
			sock->learnPeer(h, msg[LASTPORT].toUInt());
		}
		
		// Set last public IP address
		msg.remove(LASTIP);
		msg.remove(LASTPORT);
		quint32 ip = senderPeer->host.toIPv4Address();
		msg.insert(LASTIP, ip);
		msg.insert(LASTPORT, senderPeer->port);

		// Send back status
		sock->sendStatus(senderPeer);
	} else {
		// Process join DHT request

		// Update dhtStatus
		sock->updateDhtStatus(&msg);

		// If senderPeer wants to join DHT, process 
		if (msg.value(JOINDHT).toBool()) {
			sock->processJoinReq(msg);
		}
	}

	// Monger msg, or broadcast route rumor or dht join request
	if (msg.find(CHATTEXT) != msg.end()) {
		sock->monger(&msg, sock->pickPeer(*senderPeer));
	} else {
		sock->broadcast(&msg, senderPeer);
	}
}

void ChatDialog::newPrivateMsg(QString origin) {
	PrivateMessage *window = new PrivateMessage(origin);

	window->setWindowTitle(QString("Peerster (").
			       append(QString::number(sock->getThisPort())).
					     append(QString(")")));
	window->show();

	connect(window, SIGNAL(sendPM(QVariantMap)),
		sock, SLOT(gotSendPM(QVariantMap)));
}

void ChatDialog::shareFile() {
	FileSharing *share = new FileSharing();
	share->setFileMode(QFileDialog::ExistingFiles);

	share->show();

	connect(share, SIGNAL(filesSelected(QStringList)), share,
		SLOT(gotFilesSelected(QStringList)));
	connect(share, SIGNAL(shareFiles(FileSharing*)),
		sock, SLOT(gotShareFiles(FileSharing*)));
}

void ChatDialog::processSearchRep(QVariantMap msg) {
	if (msg.value(SEARCHREP).toString() == searchRequest &&
	    msg.value(ORIGIN).toString() != sock->getOriginID()) {
		QString orig = msg.value(ORIGIN).toString();
		QVariantList filenames  = msg.value(MATCHNAMES).toList();
		QVariantList metadataHashes = msg.value(MATCHIDS).toList();
		while (!filenames.isEmpty() && !metadataHashes.isEmpty()) {
			if (searchReplyArchive->find(filenames.first().toString()) ==
			    searchReplyArchive->end()) {
				// Add information to searchReplyArchive
				QPair<QByteArray, QString> pair = 
					qMakePair(metadataHashes.first().toByteArray(), orig);
				searchReplyArchive->insert(filenames.first().toString(), pair);
				
				// Display information to user
				searchResults->addItem(filenames.first().toString());
			}
			metadataHashes.removeFirst();
			filenames.removeFirst();
		}
	}
	// NOTE: discards message that is not reply fo current search
}


// FILES FUNCTIONS ------------------------------------------------

Files::Files() {
}

// DFILE FUNCTIONS ------------------------------------------------

DownloadFile::DownloadFile() {
}

// FILESHARING FUNCTIONS ------------------------------------------------
FileSharing::FileSharing() {
}

void FileSharing::gotFilesSelected(QStringList fileList) {
	qDebug() << "Sharing files" << fileList;

	QCA::init();
	if (!QCA::isSupported("sha1")) {
		qDebug() << "error: SHA-1 not supported";
		return;
	}

	QStringListIterator it(fileList);
	while (it.hasNext()) {
		Files *file = new Files();

		file->filename = it.next();
		QFile qfile(file->filename);
		if (!qfile.open(QIODevice::ReadOnly)) {
			qDebug() << "error: could not open file" << file->filename;
			continue;
		}

		file->filesize = qfile.size();

		// Compute hash for each block and append to blocklist
		QCA::Hash shaHash("sha1");
		QByteArray read;
		while (!(read = qfile.read(MAXBYTES)).isEmpty()) {
			shaHash.update(read);
			file->blocklist.append(shaHash.final().toByteArray());
			shaHash.clear();
		}

		// Take hash of blocklist
		shaHash.update(file->blocklist);
		file->blocklistHash = shaHash.final().toByteArray();
		qDebug() << " > hash of file: " << file->blocklistHash.toHex();
		
		// Get relative file name
		QStringList parts = file->filename.split("/");
		file->filename = parts.last();
		files.append(*file);
	}

	emit shareFiles(this);
}

// PRIVATEMESSAGE FUNCTIONS ------------------------------------------------

PrivateMessage::PrivateMessage() {
}

PrivateMessage::PrivateMessage(QString d) {
	msgText = new TextEdit();
	destination = d;

	QVBoxLayout *layout = new QVBoxLayout();
	QLabel *label = new QLabel(this);
	label->setText(QString("Private message with ").append(destination));
	
	layout->addWidget(label);
	layout->addWidget(msgText);

	setLayout(layout);

	// Send message upon return press
	connect(msgText, SIGNAL(returnPressed()), this, SLOT(gotReturn()));
	// Close window after sending message
	connect(this, SIGNAL(closeWindow()), this, SLOT(close()));
}

void PrivateMessage::gotReturn() {
	QVariantMap *msg = new QVariantMap();
	msg->insert(DEST, destination);
	msg->insert(CHATTEXT, msgText->toPlainText());
	msg->insert(HOPLIMIT, DEFLIM);
	emit sendPM(*msg);

	// Close private message window
	emit closeWindow();
}

// PEER FUNCTIONS ------------------------------------------------
Peer::Peer() {
}

Peer::Peer(QHostAddress h, quint16 p) {
	host = h;
	port = p;
}

bool Peer::isEqual(Peer p) {
	return this->host == p.host && this->port == p.port;
}

bool Peer::operator==(const Peer p) {
	return this->isEqual(p);
}

QString Peer::toString() {
	return host.toString().append(QString(":")).append(QString::number(port));
}

// NETSOCKET FUNCTIONS ------------------------------------------------

NetSocket::NetSocket() {
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four Peerster instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;

	seqNo = 1;
	dhtSeqNo = 1;
	noForward = false;
}

bool NetSocket::bind() {
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (quint16 p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			thisPort = p;
			qDebug() << "bound to UDP port " << p;

			QHostAddress host = QHostAddress::LocalHost;
			thisPeer = new Peer(host, p);

			// Set origin ID
			qsrand(time(NULL));
			originID = QString("Rachel").
				append(QString::number(p)).
				append(QString::number(qrand()));

			// Initalize statuses
			status = new QVariantMap();

			// Initialize archive
			archive = new QMap<QString, QMap<quint32, QVariant> >();

			// Initialize status and archive for self
			archiveNew(originID);

			// Initialize hostPorts
			hostPorts = new QMap<QString, quint16>();

			// Anti-entropy
			entropyTimer = new QTimer(this);
			connect(entropyTimer, SIGNAL(timeout()),
				this, SLOT(gotEntropyTimeout()));
			entropyTimer->start(10000);

			// Initialize timerList
			timerList = new QVector<QTimer*>();

			// Define peers
			for (quint16 i = myPortMin; i <= myPortMax; i++) {
				if (i != thisPort) {
					Peer *newP = new Peer(host, i);
					peerList.push_back(*newP);
				}
			}

			// Parse command line
			QStringList args = QCoreApplication::arguments();
			QStringListIterator it(args);
			it.next();
			while (it.hasNext()) {
				QString arg = it.next();

				// Check for noforward flag
				if (arg == QString("-noforward")) {
					noForward = true;
				} else {
					// Turn other arguments to peers
					argToPeer(arg);
				}
			}

			// Initialize routingTable
			routingTable = new QHash<QString, Peer*>();

			// Broadcast single route rumor message
			broadcast(NULL, thisPeer);

			// Route rumor timer
			routeTimer = new QTimer(this);
			connect(routeTimer, SIGNAL(timeout()),
				this, SLOT(gotRouteTimeout()));
			routeTimer->start(60000);

			// Initialize fileArchive
			fileArchive = new QMap<QString, Files>();

			// Initialize downloading information
			downloading = false;

			// Initialize DHT information
			joinDHT = false;
			emptyDHT = true;
			dhtStatus = new QMap<QString, QPair<quint32, bool> >();

			return true;
		}
	}

	qDebug() << "Oops, no ports in my default range " << myPortMin
		 << "-" << myPortMax << " available";
	return false;
}

void NetSocket::broadcast(QVariantMap *msg, Peer *senderPeer) {
	QMutableVectorIterator<Peer> itR(peerList);
	while (itR.hasNext()) {
		itR.next();
		if (!itR.value().isEqual(*senderPeer)) {
			if (msg == NULL) {
				// Send own route
				sendRoute(itR.value());
			} else {
				// Forward route or DHT join message
				sendMsg(msg, itR.value());
			}
		}
	}
}

QString NetSocket::getOriginID() {
	return originID;
}

quint32 NetSocket::getSeqNo() {
	return seqNo;
}

quint16 NetSocket::getThisPort() {
	return thisPort;
}

Peer NetSocket::getThisPeer() {
	return *thisPeer;
}

void NetSocket::incSeqNo() {
	seqNo++;
}

bool NetSocket::getNF() {
	return noForward;
}

// Archive message and update status
void NetSocket::processMsg(QVariantMap *msg) {
	QString msgOrigin = msg->value(ORIGIN).toString();
	quint32 msgSeqNo = msg->value(SEQNO).toUInt();

	// Archive msg
	(*archive)[msgOrigin].insert(msgSeqNo, *msg);

	// Update status
	// NOTE: assumes msgSeqNo is currentSeqNo
	(*status)[msgOrigin] = msgSeqNo + 1;
}

Peer* NetSocket::pickPeer(Peer sender) {
	// If no (other) peers, return NULL
	if (peerList.size() == 0 ||
	    (!sender.isEqual(*thisPeer) && peerList.size() <= 1)) {
		return NULL;
	}

	Peer *peer = new Peer();
	qsrand(time(NULL));	
	do {
		*peer = peerList[qrand() % peerList.size()];
	} while ((*peer).isEqual(sender)); // Don't send to port received from

	return peer;
}

void NetSocket::monger(QVariantMap *msg, Peer *p) {
	if (p != NULL) {
		qDebug() << originID << "mongering msg " <<
			msg->value(CHATTEXT).toString() << "to peer " <<
			p->toString();
		sendMsg(msg, *p);

		// Set timer
		QTimer *t = new QTimer(this);
		connect(t, SIGNAL(timeout()), this, SLOT(gotTimeout()));
		t->start(2000);
		timerList->push_back(t);
	}
}

// Send a datagram of msg to the given peer
void NetSocket::sendMsg(QVariantMap *msg, Peer p) {
	// Send message if this peerster is a forwarding peerster, OR
	// if msg is a status, from this peerster, or a route rumor
	if (!noForward ||
	    ((msg->find(WANT) != msg->end()) ||
	     (msg->value(ORIGIN).toString() == originID) ||
	     (msg->find(CHATTEXT) == msg->end()))) {
		QByteArray a;
		QDataStream s(&a, QIODevice::WriteOnly);
		s << *msg;
		writeDatagram(a.data(), a.size(), p.host, p.port);
	}
}

void NetSocket::sendStatus(Peer *p) {
	if (p != NULL) {
		QVariantMap *msg = new QVariantMap();
		msg->insert(WANT, *status);
		sendMsg(msg, *p);
	}
}

void NetSocket::archiveNew(QString msgOrigin) {
	if (status->find(msgOrigin) == status->end()) {
		// Add origin ID to archive
		QMap<quint32, QVariant> *originArchive =
			new QMap<quint32, QVariant>();
		archive->insert(msgOrigin, *originArchive);
		// Add origin ID to status
		status->insert(msgOrigin, 1);
	}
}

bool NetSocket::isMsgOrRouteOrDHT(QVariantMap msg, Peer *senderPeer) {
	if (msg.find(ORIGIN) == msg.end() ||
	    msg.find(SEQNO) == msg.end()) {
		return false;
	}

	QString msgOrigin = msg.value(ORIGIN).toString();

	// Preprocess msg or route
	if (msg.find(JOINDHT) == msg.end()) {
		// If msgOrigin is new, add it to archive and status
		archiveNew(msgOrigin);
		// If wanted SeqNo for msgOrigin is not the one given, declare
		// an invalid message
		if (msg.value(SEQNO) != status->value(msgOrigin)) {
			// Update routingTable if msg contains direct route
			if ((msg.value(SEQNO).toUInt() ==
			     status->value(msgOrigin).toUInt() - 1) &&
			    (msg.find(LASTIP) == msg.end()) &&
			    (msg.find(LASTPORT) == msg.end())) {
				addToRT(msgOrigin, senderPeer);
			}
			return false;
		}
	return true;
	}

	// Preprocess DHT message

	// If msgOrigin is new, add it to dhtStatus
	archiveNewDHT(msgOrigin);

	// If wanted SeqNo for msgOrigin is larger than the one given, declare an invalid message
	if (msg.value(SEQNO).toUInt() < dhtStatus->value(msgOrigin).first) {
		return false;
	}

	return true;
}

void NetSocket::archiveNewDHT(QString msgOrigin) {
	if (dhtStatus->find(msgOrigin) == dhtStatus->end()) {
		// Add origin ID to status
		QPair<quint32, bool> *item = new QPair<quint32, bool>();
		dhtStatus->insert(msgOrigin, *item);
	}
}

bool NetSocket::isP2P(QVariantMap msg) {
	if (msg.find(DEST) == msg.end() ||
	    msg.find(ORIGIN) == msg.end() ||
	    msg.find(HOPLIMIT) == msg.end() ||
	    !(msg.find(CHATTEXT) == msg.end() ||
	      msg.find(BLOCKREQ) == msg.end() ||
	      (msg.find(BLOCKREPLY) == msg.end() &&
	       msg.find(DATA) == msg.end()) ||
	      (msg.find(SEARCHREP) == msg.end() &&
	       msg.find(MATCHNAMES) == msg.end() &&
	       msg.find(MATCHIDS) == msg.end()))) {
		return false;
	}
	return true;
}

bool NetSocket::isSearchReq(QVariantMap msg) {
	if (msg.find(ORIGIN) == msg.end() ||
	    msg.find(SEARCH) == msg.end() ||
	    msg.find(BUDGET) == msg.end()) {
		return false;
	}
	return true;
}

void NetSocket::processStatus(QVariantMap msg, Peer senderPeer) {
	// Turn off first running timer
	if (!timerList->isEmpty()) {
		timerList->front()->stop();
		timerList->pop_front();
	}

	QVariantMap inputStatus = msg[WANT].toMap();

	// Try to send a message senderPort doesn't have
	QMapIterator<QString, QVariant> it(*status);
	while (it.hasNext()) {
		it.next();

		if (it.value().toUInt() > 1) {
			QVariantMap *toSend = new QVariantMap();
			if (inputStatus.find(it.key()) == inputStatus.end()) {
				// qDebug() << "key" <<  it.key();
				// Send first message from origin senderPort
				// doesn't have
				*toSend = ((*archive)[it.key()])[1].toMap();
				sendMsg(toSend, senderPeer);
				return;
			} else if (inputStatus.value(it.key()).toUInt() <
				   it.value().toUInt()) {
				// qDebug() << "key" <<  it.key() << "value" <<
				//	inputStatus.value(it.key()).toUInt();

				// Send next message from origin senderPort has
				*toSend = ((*archive)[it.key()])
					[inputStatus.value(it.key()).toUInt()].
					toMap();
				sendMsg(toSend, senderPeer);
				return;
			}
		}
	}

	// Ask for a missing message from senderPort
	QMapIterator<QString, QVariant> it2(inputStatus);
	while (it2.hasNext()) {
		it2.next();

		// If inputStatus has a new msgOrigin, add it to archive and status
		archiveNew(it2.key());

		
		if (it2.value().toUInt() > status->value(it2.key()).toUInt()) {
			sendStatus(&senderPeer);
			return;
		}
	}

	qsrand(time(NULL));
	if (qrand() % 2 == 0) {
		sendStatus(pickPeer(senderPeer));
		return;
	}
}

void NetSocket::gotTimeout() {
	timerList->front()->stop();
	timerList->pop_front();
	sendStatus(pickPeer(*thisPeer));
}

void NetSocket::gotEntropyTimeout() {
	entropyTimer->start(10000);
	sendStatus(pickPeer(*thisPeer));
}

void NetSocket::gotRouteTimeout() {
	routeTimer->start(60000);
	broadcast(NULL, thisPeer);
}

void NetSocket::gotRetransmit() {
	sendMsg(dfile->msg, dfile->dest);
	dfile->retransmit->start(2000);
}

void NetSocket::sendRoute(Peer p) {
	qDebug() << originID << "sending route to peer " << p.toString();

	QVariantMap *msg = new QVariantMap();
	msg->insert(ORIGIN, originID);
	// Send SeqNo of last sent message
	msg->insert(SEQNO, seqNo++);
	// Archive msg and update status
	processMsg(msg);
	// Send route rumor to peer
	sendMsg(msg, p);
}

void NetSocket::learnPeer(QHostAddress sender, quint16 senderPort) {
	Peer *newP = new Peer(sender, senderPort);
	if (!peerList.contains(*newP)) {
		peerList.push_back(*newP);
	}
}

void NetSocket::argToPeer(QString arg) {
	QStringList sections = arg.split(':');
	if (sections.size() != 2) {
		// qDebug() << "invalid peer " << arg;
		return;
	}

	QHostAddress host;
	if (host.setAddress(sections[0])) {
		learnPeer(host, sections[1].toUInt());
	} else {
		hostPorts->insert(sections[0], sections[1].toUInt());
		QHostInfo::lookupHost(sections[0],
				      this, SLOT(lookedUp(QHostInfo)));
	}
}

void NetSocket::lookedUp(QHostInfo hostInfo) {
	QList<QHostAddress> addresses = hostInfo.addresses();
	if (!addresses.isEmpty()) {
		QListIterator<QHostAddress> it(addresses);
		while (it.hasNext()) {
			if (hostPorts->find(hostInfo.hostName()) !=
			    hostPorts->end()) {
				learnPeer(it.next(),
					  hostPorts->value(hostInfo.hostName()));
				hostPorts->erase(hostPorts->find(hostInfo.
								 hostName()));
			}
		}
	}
}

void NetSocket::addToRT(QString origin, Peer *p) {
	if (origin != originID) {
		// Remove any instances of the origin in the routingTable
		if (routingTable->remove(origin) == 0) {
			// Add to originList if new
			originList->addItem(origin);
			originList->setCurrentIndex(-1);
		}
		// Add to routingTable
		routingTable->insert(origin, p);
	}
}

void NetSocket::resetOL() {
	originList->setCurrentIndex(-1);
}

void NetSocket::gotSendPM(QVariantMap msg) {
	msg.insert(ORIGIN, originID);

	// Find "Dest" originID in routing table
	Peer *dest = routingTable->value(msg.value(DEST).toString());
	// Send to that peer
	sendMsg(&msg, *dest);
}

void NetSocket::forwardP2P(QVariantMap msg) {
	// Decrement hop
	msg[HOPLIMIT] = msg.value(HOPLIMIT).toUInt() - 1;
	// qDebug() << "hopLim now " << msg.value(HOPLIMIT).toUInt();

	// Send to appropriate peer from routingTable
	sendMsg(&msg, *(routingTable->value(msg.value(DEST).toString())));
}

void NetSocket::gotShareFiles(FileSharing *share) {
	QVectorIterator<Files> it(share->files);
	while (it.hasNext()) {
		// Store each file to be shared in internal database
		Files file = it.next();
		if (fileArchive->find(file.filename) == fileArchive->end()) {
			fileArchive->insert(file.filename, file);
		} else {
			qDebug() << "file" << file.filename << "already in fileArchive";
		}
	}
}

bool NetSocket::isDownloading() {
	return downloading;
}

QByteArray NetSocket::getDfileBlockReq() {
	return dfile->msg->value(BLOCKREQ).toByteArray();
}

QString NetSocket::getTargetNode() {
	return dfile->targetNode;
}

void NetSocket::gotReqToDownload(QPair<QString, QPair<QByteArray, QString> > pair) {
	if (routingTable->find(pair.second.second) == routingTable->end()) {
		qDebug() << " > invalid target node";
		return;
	}

	// Form message
	QVariantMap *msg = new QVariantMap();
	msg->insert(DEST, pair.second.second);
	msg->insert(BLOCKREQ, pair.second.first);
	msg->insert(ORIGIN, originID);
	msg->insert(HOPLIMIT, DEFLIM);

	// Find originID in routing table
	Peer *dest = routingTable->value(msg->value(DEST).toString());

	// Send to that peer
	sendMsg(msg, *dest);

	// Note file as awaiting download
	dfile = new DownloadFile();
	dfile->targetNode = pair.second.second;
	dfile->blocksDownloaded = 0;
	dfile->dest = *dest;
	dfile->msg = msg;
	dfile->file = new Files();

	// Set file name as relative file name
	QStringList parts = pair.first.split("/");
	dfile->file->filename = QString("download_").append(parts.last());

	// Every 2 seconds, retransmit request
	dfile->retransmit = new QTimer(this);
	connect(dfile->retransmit, SIGNAL(timeout()),
		this, SLOT(gotRetransmit()));
	dfile->retransmit->start(2000);
	return;
}

QByteArray NetSocket::findBlock(QByteArray blockReq) {
	QMapIterator<QString, Files> it(*fileArchive);
	QByteArray *block = new QByteArray();
	while (it.hasNext()) {
		Files file = it.next().value();
		// Return blocklist metafile if given a blocklistHash
		if (file.blocklistHash == blockReq) {
			qDebug() << originID << "found blocklist metafile";
			*block = QByteArray(file.blocklist);
			return *block;
		}
		// Return block of data if given a blocklist metafile chunk
		for (qint64 i = 1; i <= file.filesize; i++) {
			qint64 blockIndex = (i-1) * 20;
			if (file.blocklist.indexOf(blockReq, blockIndex) == blockIndex) {
				QFile readF(file.filename);
				readF.open(QIODevice::ReadOnly);
				if (!readF.seek((i-1)*MAXBYTES)) {
					qDebug() << "error reading from" << file.filename;
				}
				*block = readF.read(MAXBYTES);
				qDebug() << originID << "found data block" << i-1;
				return *block;
			}
		}
	}
	qDebug() << originID << "did not find blockReq";
	return *block;
}

void NetSocket::processBlockReply(QByteArray data) {
	dfile->retransmit->stop();

	if (dfile->file->blocklist.isEmpty()) {
		// Save blocklist metadata
		dfile->file->blocklist = data;
		// Set filesize
		dfile->file->filesize = data.size() / 20;
	} else {
		// Write block to file
		if (dfile->blocksDownloaded == 0) {
			qDebug() << originID << "saving file as" << dfile->file->filename;
			dfile->writeFile = new QFile(dfile->file->filename);
			dfile->writeFile->open(QIODevice::WriteOnly);
		}
		dfile->writeFile->write(data);
		// Update count of blocks downloaded
		dfile->blocksDownloaded += 1;
	}

	if (dfile->blocksDownloaded == dfile->file->filesize) {
		// Indicate end of download and close file
		downloading = false;
		dfile->writeFile->close();
	} else {
		// Form and send next block request
		dfile->msg->remove(BLOCKREQ);
		QByteArray *newBlockReq = new QByteArray();
		for (int i = 0; i < 20; i++) {
			newBlockReq->append(dfile->file->blocklist.
					    at(20*(dfile->blocksDownloaded)+i));
		}
		dfile->msg->insert(BLOCKREQ, *newBlockReq);

		sendMsg(dfile->msg, dfile->dest);

		dfile->retransmit->start(2000);
	}
}

void NetSocket::processSearchReq(QVariantMap msg, Peer p) {
	QVariantMap *rep = new QVariantMap();
	rep->insert(DEST, msg.value(ORIGIN));
	rep->insert(ORIGIN, originID);
	rep->insert(HOPLIMIT, DEFLIM);
	rep->insert(SEARCHREP, msg.value(SEARCH));

	qDebug() << originID << "received search request for"
		 << msg.value(SEARCH).toString();

	QVariantList *names = new QVariantList();
	QVariantList *ids = new QVariantList();

	QStringList strings = msg.value(SEARCH).toString().split(QRegExp("\\s+"));
	QMapIterator<QString, Files> it(*fileArchive);
	while (it.hasNext()) {
		QString filename = it.next().key();

		QStringListIterator lit(strings);
		while (lit.hasNext()) {
			if (filename.contains(lit.next(), Qt::CaseInsensitive)) {
				names->push_back(it.value().filename);
				ids->push_back(it.value().blocklistHash);
				break;
			}
		}
	}
	rep->insert(MATCHNAMES, *names);
	rep->insert(MATCHIDS, *ids);
	sendMsg(rep, p);
	return;
}

void NetSocket::gotStartSearchFor(QPair<QString, quint32> pair) {
	QVariantMap *msg = new QVariantMap();
	msg->insert(ORIGIN, originID);
	msg->insert(SEARCH, pair.first);
	msg->insert(BUDGET, pair.second);
	sendByBudget(*msg);
}

void NetSocket::sendByBudget(QVariantMap msg) {
	int numPeers = peerList.size();
	quint32 budget = msg.value(BUDGET).toUInt();
	int fullBudgetPeers = budget % numPeers;
	quint32 fullBudget = budget / numPeers + 1;
	int smallBudgetPeers = numPeers - fullBudgetPeers;
	quint32 smallBudget = fullBudget - 1;

	QVectorIterator<Peer> it(peerList);
	while (it.hasNext()) {
		Peer p = it.next();
		if (fullBudgetPeers > 0) {
			msg[BUDGET] = fullBudget;
			fullBudgetPeers--;
			qDebug() << originID << "sent message to"
				 << p.toString() << "with budget"
				 << fullBudget;
			sendMsg(&msg, p);
		} else if (smallBudgetPeers > 0 && smallBudget > 0) {
			msg[BUDGET] = smallBudget;
			smallBudgetPeers--;
			qDebug() << originID << "sent message to"
				 << p.toString() << "with budget"
				 << smallBudget;
			sendMsg(&msg, p);
		}
	}
}

void NetSocket::changedDHTPreference(int state) {
	if (state == Qt::Checked) {
		joinDHT = true;
		// Add any positive join requests in dhtStatus to DHT
		QMapIterator<QString, QPair<quint32, bool> > it(*dhtStatus);
		while (it.hasNext()) {
			it.next();

			if (it.value().second == true) {
				// TODO(rachel): insertToDHT(it.key());
			}
		}
	} else {
		joinDHT = false;
	}

	// Create and send DHT join message
	QVariantMap *msg = new QVariantMap();
	msg->insert(ORIGIN, originID);
	msg->insert(SEQNO, dhtSeqNo++);
	msg->insert(JOINDHT, joinDHT);

	// Update dhtStatus
	updateDhtStatus(msg);

	// Broadcast
	broadcast(msg, thisPeer);
}

void NetSocket::updateDhtStatus(QVariantMap *msg) {
	// NOTE: assumed msg SEQNO is higher than current value in dhtStatus
	(*dhtStatus)[msg->value(ORIGIN).toString()].first = msg->value(SEQNO).toUInt();
	(*dhtStatus)[msg->value(ORIGIN).toString()].second = msg->value(JOINDHT).toBool();
}

// TODO(rachel): remove this when isEmptyDHT is implemented
void NetSocket::gotJoinedDHT() {
	emptyDHT = false;
}

bool NetSocket::isEmptyDHT() {
	// TODO(terin): fill in this function
	return emptyDHT;
}

void NetSocket::processJoinReq(QVariantMap msg) {
	// Check that user wants to join DHT
	if (joinDHT) {
		// Join DHT if haven't already done so
		if (isEmptyDHT()) {
			emit joinedDHT();
		}
		// Add msg origin to DHT
		// TODO(rachel): insertToDHT(msg->value(ORIGIN).toString());
	}
}

// MAIN ------------------------------------------------

int main(int argc, char **argv) {
	// Initialize Qt toolkit
	QApplication app(argc,argv);

	QCA::Initializer qcainit;

	// Create an initial chat dialog window
	ChatDialog dialog;
	dialog.show();

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

