/*
 *   Copyright (C) 2007 Tobias Koenig <tokoe@kde.org>
 *   Copyright  2008 by Anne-Marie Mahfouf <annma@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "noaaprovider.h"

#include <QRegExp>
#include <QImage>
#include <QRegularExpression>
#include <QDebug>

#include <kio/job.h>

class NOAAProvider::Private
{
  public:
    Private( NOAAProvider *parent )
      : mParent( parent )
    {
    }

    void pageRequestFinished( KJob* );
    void imageRequestFinished( KJob* );
    void parsePage();

    NOAAProvider *mParent;
    QImage mImage;
};

void NOAAProvider::Private::pageRequestFinished( KJob* _job )
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>( _job );
    if (job->error()) {
        emit mParent->error( mParent );
        return;
    }

    const QString data = QString::fromUtf8( job->data() );

    // Using regular expression could be fragile in such case, but the HTML
    // NOAA page itself is not a valid XML file and unfortunately it could
    // not be parsed successfully till the content we want. And we do not want
    // to use heavy weight QtWebkit. So we use QRegularExpression to capture
    // the wanted url here.
    QString url;
    QRegularExpression re(QStringLiteral("_curPic = (.*?)</script>"));
    auto result = re.match(data);
    if (result.hasMatch()) {
        url = QLatin1String("http://www.nnvl.noaa.gov/") + result.captured(1);
    }
    if (url.isEmpty()) {
        emit mParent->error( mParent );
        return;
    }

    KIO::StoredTransferJob *imageJob = KIO::storedGet( QUrl(url), KIO::NoReload, KIO::HideProgressInfo );
    mParent->connect( imageJob, &KIO::StoredTransferJob::finished, mParent, [this] (KJob* job) {
        imageRequestFinished(job);
    });
}

void NOAAProvider::Private::imageRequestFinished( KJob *_job )
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>( _job );
    if ( job->error() ) {
	emit mParent->error( mParent );
	return;
    }

    mImage = QImage::fromData( job->data() );
    emit mParent->finished( mParent );
}

NOAAProvider::NOAAProvider( QObject *parent, const QVariantList &args )
    : PotdProvider( parent, args ), d( new Private( this ) )
{
    QUrl url( QLatin1String( "http://www.nnvl.noaa.gov/imageoftheday.php" ) );
    KIO::StoredTransferJob *job = KIO::storedGet( url, KIO::NoReload, KIO::HideProgressInfo );
    connect( job, &KIO::StoredTransferJob::finished, this, [this] (KJob *job) {
        d->pageRequestFinished(job);
    });
}

NOAAProvider::~NOAAProvider()
{
    delete d;
}

QImage NOAAProvider::image() const
{
    return d->mImage;
}

K_PLUGIN_FACTORY_WITH_JSON(NOAAProviderFactory, "noaaprovider.json", registerPlugin<NOAAProvider>();)

#include "noaaprovider.moc"
