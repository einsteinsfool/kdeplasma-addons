/*
 * Copyright (C) 2007, 2008 Petri Damsten <damu@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "news.h"

#include <QGraphicsSceneDragDropEvent>
#include <QFile>
#include <QColor>
#include <QDomDocument>
#include <QGraphicsLinearLayout>
#include <QWebFrame>
#include <QWebPage>
#include <QFileInfo>

#include <KConfigDialog>
#include <KColorScheme>
#include <KStandardDirs>
#include <KRun>

#include <Plasma/Containment>
#include <Plasma/Theme>
#include <Plasma/WebView>

#define BEGIN "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n" \
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"" \
"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n" \
"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n" \
"<head>\n" \
" <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n" \
" <link rel=\"stylesheet\" type=\"text/css\" href=\"news.css\" />\n" \
" <title>Plasma News</title>\n" \
"</head>\n" \
"<body>\n"

#define BEGIN_TABLE "<table>\n"
#define HEADER_LINE "<tr class=\"header_line\">" \
                    "<td colspan=\"2\" class=\"header\"><a href=\"%1\">%2</a></td></tr>\n"
#define DATE_LINE   "<tr class=\"date_line\"><td colspan=\"2\" class=\"date\">%1</td></tr>\n"
#define LINE        "<tr class=\"line-%1\">" \
                    "<td colspan=\"2\" class=\"text\"><a href=\"%2\">%3</a></td></tr>\n"
#define TIME_LINE   "<tr class=\"line-%1\"><td class=\"time\">%2</td>" \
                    "<td class=\"text\"><a href=\"%3\">%4</a></td></tr>\n"
#define TITLE       "<span class=\"title\">%1</span>"
#define TITLE_DESC  "<span class=\"title_with_description\">%1</span><br/>"
#define DESCRIPTION "<span class=\"description\">%1</span>"
#define END_TABLE   "</table>\n"
#define END         "</body>\n</html>\n"

#define CSS ".header { font-size:11pt; font-weight:bold; color:%1; background-color:%2; }\n" \
            ".date { font-size:10pt; font-weight:bold; color:%1; background-color:%2; }\n" \
            ".line-0 { font-size:10pt; color:%1; background-color:%3; }\n" \
            ".line-1 { font-size:10pt; color:%1; background-color:%4; }\n" \
            ".time, .title, .description { }\n" \
            ".text { width:100%; }\n" \
            ".title_with_description { font-weight:bold; }\n" \
            "a { text-decoration:none; color:%1 }\n" \
            "table { width:100%; border-spacing:0px; }\n" \
            "td { vertical-align: top; }\n" \
            "body { margin:0px; background-color:%3 }\n"

News::News(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    setHasConfigurationInterface(true);
    resize(QSize(370, 440));
    // The default width is OK, we can benefit from more vertical space
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

News::~News()
{
}

void News::init()
{
    KConfigGroup cg = config();
    m_interval = cg.readEntry("interval", 30);
    m_showTimestamps = cg.readEntry("showTimestamps", true);
    m_showTitles = cg.readEntry("showTitles", true);
    m_showDescriptions = cg.readEntry("showDescriptions", false);

    m_feeds = cg.readEntry("feeds", QStringList("http://www.kde.org/dotkdeorg.rdf"));

    m_layout = new QGraphicsLinearLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_news = new Plasma::WebView(this);
    connect(m_news->page(), SIGNAL(linkClicked(const QUrl&)),
            this, SLOT(linkActivated(const QUrl&)));
    m_news->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    m_layout->addItem(m_news);
    setLayout(m_layout);

    setAcceptDrops(true);
    makeStylesheet();
    connectToEngine();

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(makeStylesheet()));
}

void News::connectToEngine()
{
    if (!m_feeds.isEmpty()) {
        Plasma::DataEngine* engine = dataEngine("rss");
        if( engine) {

            QString feedstring;

            foreach (const QString& feed, m_feeds) {
                QByteArray ba = QUrl::toPercentEncoding(feed, ":/");
                feedstring.append(ba);
                feedstring.append(" ");
            }
            m_feedstring = feedstring;
            engine->connectSource(feedstring, this, m_interval * 60 * 1000);
        }
        else
            kDebug()<<"Can not connect to dataengine rss";
    }
}

void News::makeStylesheet()
{
    QString search = "desktoptheme/" + Plasma::Theme::defaultTheme()->themeName() +
                     "/stylesheets/news.css";
    QString path =  KStandardDirs::locate("data", search);
    if (path.isEmpty()) {
        KColorScheme plasmaColorTheme = KColorScheme(QPalette::Active, KColorScheme::View,
                Plasma::Theme::defaultTheme()->colorScheme());
        QColor headerColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::HighlightColor);
        QColor textColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
        QColor backgroundColor =
                Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor);
        QColor altBackgroundColor =
                Plasma::Theme::defaultTheme()->color(Plasma::Theme::ButtonBackgroundColor);

        QString css = QString(CSS).arg(textColor.name())
                                .arg(headerColor.name())
                                .arg(backgroundColor.name())
                                .arg(altBackgroundColor.name());
        KStandardDirs dirs;
        m_cssDir = dirs.saveLocation("data", "plasma_applet_news");

        QFile file(m_cssDir + "/news.css");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(css.toUtf8());
        }
    } else {
        m_cssDir = QFileInfo(path).absolutePath() + '/';
    }

    QWebFrame* frame = m_news->mainFrame();
    if (frame) {
        frame->setHtml(frame->toHtml(), m_cssDir);
    }
}

QMap<QString, QString> News::akregatorFeeds()
{
    QMap<QString, QString> result;
    QString file = KStandardDirs::locate("data", "akregator/data/feeds.opml");
    if (!file.isEmpty()) {
        QFile f(file);
        if (f.open(QIODevice::ReadOnly)) {
            QDomDocument doc;
            if (doc.setContent(&f)) {
                QDomNodeList nodes = doc.elementsByTagName("outline");
                for (int i = 0; i < nodes.count(); ++i) {
                    QDomElement element = nodes.item(i).toElement();
                    QString title = element.attribute("title");
                    if (!title.isEmpty()) {
                        result[title] = element.attribute("xmlUrl");
                    }
                }
            }
            f.close();
        }
    }
    return result;
}

void News::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget();
    ui.setupUi(widget);
    parent->setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);
    parent->addPage(widget, parent->windowTitle(), "application-rss+xml");

    connect(ui.feedComboBox, SIGNAL(editTextChanged(const QString&)),
            this, SLOT(feedTextChanged(const QString&)));
    connect(parent, SIGNAL(accepted()), this, SLOT(configAccepted()));
    connect(ui.addFeed, SIGNAL(clicked()), this, SLOT(addFeed()));
    connect(ui.removeFeed, SIGNAL(clicked()), this, SLOT(removeFeed()));

    m_defaultFeeds = akregatorFeeds();
    ui.feedComboBox->clear();
    feedTextChanged(QString());
    ui.feedList->clear();
    foreach (const QString& name, m_defaultFeeds.keys()) {
        ui.feedComboBox->addItem(name);
    }

    ui.intervalSpinBox->setValue(m_interval);

    ui.timestampCheckBox->setChecked(m_showTimestamps);
    ui.titlesCheckBox->setChecked(m_showTitles);
    ui.descriptionsCheckBox->setChecked(m_showDescriptions);

    ui.feedList->addItems(m_feeds);
}

void News::feedTextChanged(const QString& text)
{
    ui.addFeed->setEnabled(!text.isEmpty());
}

void News::addFeed()
{
    if (!ui.feedComboBox->currentText().isEmpty()) {
        QString url = ui.feedComboBox->currentText();
        if (m_defaultFeeds.keys().contains(url)) {
            url = m_defaultFeeds[url];
        }
        bool found = false;
        for (int i = 0; i < ui.feedList->count(); i++) {
            QString feed = ui.feedList->item(i)->text();
            if (feed == url) {
                found = true;
            }
        }
        if (!found) {
            ui.feedList->addItem(url);
        }
        ui.removeFeed->setEnabled(true);
    }
}

void News::removeFeed()
{
    int row = ui.feedList->currentRow();
    if (row != -1) {
        ui.feedList->takeItem(row);
        if (ui.feedList->count() == 0) {
            ui.removeFeed->setEnabled(false);
        }
    }
}

void News::configAccepted()
{
    m_interval = ui.intervalSpinBox->value();
    m_showTimestamps = ui.timestampCheckBox->isChecked();
    m_showTitles = ui.titlesCheckBox->isChecked();
    m_showDescriptions = ui.descriptionsCheckBox->isChecked();

    m_feeds.clear();
    QString feed;
    for (int i = 0; i < ui.feedList->count(); i++) {
        feed = ui.feedList->item(i)->text();
        if (m_defaultFeeds.keys().contains(feed)) {
            feed = m_defaultFeeds[feed];
        }
        m_feeds << feed;
    }

    KConfigGroup cg = config();
    cg.writeEntry("feeds", m_feeds);
    cg.writeEntry("interval", m_interval);
    cg.writeEntry("showTimestamps", m_showTimestamps);
    cg.writeEntry("showTitles", m_showTitles);
    cg.writeEntry("showDescriptions", m_showDescriptions);

    connectToEngine();
    dataUpdated(m_feedstring, m_dataCache);
}

void News::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (KUrl::List::canDecode(event->mimeData())) {
        KUrl::List urls = KUrl::List::fromMimeData(event->mimeData());

        if (urls.count() > 0) {
            event->accept();
            m_feeds.clear();
            foreach (const KUrl &feed, urls) {
                m_feeds << feed.prettyUrl();
            }
            m_interval = 30;
            connectToEngine();
        }
    }
}

void News::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    if (source == m_feedstring && !data.isEmpty()) {
        m_dataCache = data;
        QDateTime previous = QDateTime::currentDateTime();
        QString html;
        QVariantList items = data["items"].toList();
        uint columns = 1;
        uint i = 0;

        html += BEGIN;
        html += BEGIN_TABLE;
        html += QString(HEADER_LINE).arg(data["link"].toString())
                                    .arg(data["title"].toString());

        foreach (const QVariant &tmp, items) {
            QMap<QString, QVariant> item = tmp.toMap();
            if (item["time"].toUInt() != 0) {
                columns = 2;
            } else {
                columns = 1;
            }
            uint timeStamp = item["time"].toUInt();
            QString title = item["title"].toString();
            QString description = item["description"].toString().replace(QRegExp("<[^>]*>"), "");
            QString line;

            if (title != 0 && m_showTitles) {
                if (description != 0 && m_showDescriptions) {
                    line += QString(TITLE_DESC).arg(title);
                } else {
                    line += QString(TITLE).arg(title);
                }
            }
            if (description != 0 && m_showDescriptions) {
                line += QString(DESCRIPTION).arg(description);
            }
            if (timeStamp != 0 && m_showTimestamps) {
                QDateTime time;
                time.setTime_t(timeStamp);
                if (previous.date() != time.date()) {
                    html += QString(DATE_LINE).arg(time.date().toString());
                    i = 0;
                }
                html += QString(TIME_LINE).arg(i % 2)
                                            .arg(time.time().toString("HH:mm"))
                                            .arg(item["link"].toString())
                                            .arg(line);
                previous = time;
            } else {
                html += QString(LINE).arg(i % 2)
                                     .arg(item["link"].toString())
                                     .arg(line);
            }
            ++i;
        }
        html += END_TABLE;
        html += END;
        m_news->setHtml(html, m_cssDir);
    }
}

void News::linkActivated(const QUrl& link)
{
   KRun::runUrl(KUrl(link), "text/html", 0);
}

#include "news.moc"
