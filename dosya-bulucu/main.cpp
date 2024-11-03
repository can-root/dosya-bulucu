#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QDirIterator>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>
#include <QFile>
#include <QTextStream>

class DosyaBulucu : public QWidget {
    Q_OBJECT

public:
    DosyaBulucu(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *anaLayout = new QVBoxLayout(this);
        QTabWidget *sekmeWidget = new QTabWidget(this);

        QWidget *dosyaAdiSekmesi = new QWidget(this);
        QVBoxLayout *dosyaAdiLayout = new QVBoxLayout(dosyaAdiSekmesi);

        dosyaAdiGirdisi = new QLineEdit(this);
        dosyaAdiGirdisi->setPlaceholderText("Dosya adını girin");
        dosyaAdiLayout->addWidget(dosyaAdiGirdisi);

        QPushButton *dizinSecButonu = new QPushButton("Dizin Seç", this);
        dosyaAdiLayout->addWidget(dizinSecButonu);
        connect(dizinSecButonu, &QPushButton::clicked, this, &DosyaBulucu::dizinSec);

        QPushButton *bulButonu = new QPushButton("Bul", this);
        dosyaAdiLayout->addWidget(bulButonu);
        connect(bulButonu, &QPushButton::clicked, this, &DosyaBulucu::dosyaBul);

        sonuclarTextEdit = new QTextEdit(this);
        sonuclarTextEdit->setReadOnly(true);
        dosyaAdiLayout->addWidget(sonuclarTextEdit);

        dosyaAdiSekmesi->setLayout(dosyaAdiLayout);
        sekmeWidget->addTab(dosyaAdiSekmesi, "Dosya Adı ile Ara");

        QWidget *icerikSekmesi = new QWidget(this);
        QVBoxLayout *icerikLayout = new QVBoxLayout(icerikSekmesi);

        icerikAnahtarKelimeGirdisi = new QLineEdit(this);
        icerikAnahtarKelimeGirdisi->setPlaceholderText("İçeriğinde geçen kelimeleri girin (virgülle ayrılmış)");
        icerikLayout->addWidget(icerikAnahtarKelimeGirdisi);

        icerikDosyaAdiGirdisi = new QLineEdit(this);
        icerikDosyaAdiGirdisi->setPlaceholderText("Dosya adını girin");
        icerikLayout->addWidget(icerikDosyaAdiGirdisi);

        icerikUzantiGirdisi = new QLineEdit(this);
        icerikUzantiGirdisi->setPlaceholderText("Uzatıyı girin (örn. .txt)");
        icerikLayout->addWidget(icerikUzantiGirdisi);

        QPushButton *icerikDizinSecButonu = new QPushButton("Dizin Seç", this);
        icerikLayout->addWidget(icerikDizinSecButonu);
        connect(icerikDizinSecButonu, &QPushButton::clicked, this, &DosyaBulucu::icerikDizinSec);

        QPushButton *icerikAraButonu = new QPushButton("İçerik Ara", this);
        icerikLayout->addWidget(icerikAraButonu);
        connect(icerikAraButonu, &QPushButton::clicked, this, &DosyaBulucu::icerikBul);

        icerikSonuclarTextEdit = new QTextEdit(this);
        icerikSonuclarTextEdit->setReadOnly(true);
        icerikLayout->addWidget(icerikSonuclarTextEdit);

        icerikSekmesi->setLayout(icerikLayout);
        sekmeWidget->addTab(icerikSekmesi, "İçeriğe Göre Ara");

        anaLayout->addWidget(sekmeWidget);
        setLayout(anaLayout);
        setWindowTitle("Dosya Bulucu");
        resize(400, 600);
    }

private slots:
    void dizinSec() {
        QString dizin = QFileDialog::getExistingDirectory(this, "Dizin Seç");
        if (!dizin.isEmpty()) {
            dosyaBaslangicDizini = dizin;
        }
    }

    void icerikDizinSec() {
        QString dizin = QFileDialog::getExistingDirectory(this, "Dizin Seç");
        if (!dizin.isEmpty()) {
            icerikBaslangicDizini = dizin;
        }
    }

    void dosyaBul() {
        if (dosyaBaslangicDizini.isEmpty()) {
            QMessageBox::warning(this, "Uyarı", "Lütfen bir dizin seçin.");
            return;
        }

        QString dosyaAdi = dosyaAdiGirdisi->text();

        if (dosyaAdi.isEmpty()) {
            QMessageBox::warning(this, "Uyarı", "Lütfen bir dosya adı girin.");
            return;
        }

        sonuclarTextEdit->clear();
        QDir dizin(dosyaBaslangicDizini);
        QFileInfoList dosyalar = dizin.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        for (const QFileInfo &dosyaBilgisi : dosyalar) {
            if (dosyaBilgisi.fileName().contains(dosyaAdi)) {
                sonuclarTextEdit->append(dosyaBilgisi.absoluteFilePath());
            }
        }

        QDirIterator it(dosyaBaslangicDizini, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            if (it.fileInfo().fileName().contains(dosyaAdi)) {
                sonuclarTextEdit->append(it.fileInfo().absoluteFilePath());
            }
        }

        if (sonuclarTextEdit->toPlainText().isEmpty()) {
            sonuclarTextEdit->setPlainText("Hiçbir dosya bulunamadı.");
        }
    }

    void icerikBul() {
        if (icerikBaslangicDizini.isEmpty()) {
            QMessageBox::warning(this, "Uyarı", "Lütfen bir dizin seçin.");
            return;
        }

        QString dosyaAdi = icerikDosyaAdiGirdisi->text();
        QString uzanti = icerikUzantiGirdisi->text();
        QString anahtarKelimeler = icerikAnahtarKelimeGirdisi->text();

        if (dosyaAdi.isEmpty() || uzanti.isEmpty() || anahtarKelimeler.isEmpty()) {
            QMessageBox::warning(this, "Uyarı", "Lütfen dosya adı, uzantı ve içerik kelimeleri girin.");
            return;
        }

        QStringList anahtarKelimeListesi = anahtarKelimeler.split(',', QString::SkipEmptyParts);
        icerikSonuclarTextEdit->clear();
        QDir dizin(icerikBaslangicDizini);
        QFileInfoList dosyalar = dizin.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        for (const QFileInfo &dosyaBilgisi : dosyalar) {
            if (dosyaBilgisi.fileName().contains(dosyaAdi) && dosyaBilgisi.suffix() == uzanti.mid(1)) {
                if (icerikAnahtarKelimeKontrol(dosyaBilgisi, anahtarKelimeListesi)) {
                    icerikSonuclarTextEdit->append(dosyaBilgisi.absoluteFilePath());
                }
            }
        }

        QDirIterator it(icerikBaslangicDizini, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            if (it.fileInfo().fileName().contains(dosyaAdi) && it.fileInfo().suffix() == uzanti.mid(1)) {
                if (icerikAnahtarKelimeKontrol(it.fileInfo(), anahtarKelimeListesi)) {
                    icerikSonuclarTextEdit->append(it.fileInfo().absoluteFilePath());
                }
            }
        }

        if (icerikSonuclarTextEdit->toPlainText().isEmpty()) {
            icerikSonuclarTextEdit->setPlainText("Hiçbir dosya bulunamadı.");
        }
    }

private:
    bool icerikAnahtarKelimeKontrol(const QFileInfo &dosyaBilgisi, const QStringList &anahtarKelimeListesi) {
        QFile dosya(dosyaBilgisi.absoluteFilePath());
        if (!dosya.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }

        QTextStream in(&dosya);
        while (!in.atEnd()) {
            QString satir = in.readLine();
            for (const QString &anahtarKelime : anahtarKelimeListesi) {
                if (satir.contains(anahtarKelime.trimmed(), Qt::CaseInsensitive)) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    QLineEdit *dosyaAdiGirdisi;
    QLineEdit *icerikAnahtarKelimeGirdisi;
    QLineEdit *icerikDosyaAdiGirdisi;
    QLineEdit *icerikUzantiGirdisi;
    QTextEdit *sonuclarTextEdit;
    QTextEdit *icerikSonuclarTextEdit;
    QString dosyaBaslangicDizini;
    QString icerikBaslangicDizini;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFile dosya("styles.css");
    if (dosya.open(QFile::ReadOnly)) {
        QString stilSayfasi = QTextStream(&dosya).readAll();
        app.setStyleSheet(stilSayfasi);
    }

    DosyaBulucu dosyaBulucu;
    dosyaBulucu.show();
    return app.exec();
}

#include "main.moc"
