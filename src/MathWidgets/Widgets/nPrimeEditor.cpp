#include <mathwidgets.h>

N::PrimeEditor:: PrimeEditor (QWidget * parent,Plan * p)
               : Widget      (          parent,       p)
               , PrimeId     (0                        )
               , ID          (NULL                     )
               , Prime       (NULL                     )
{
  QStringList headers                             ;
  setWindowTitle (tr("Prime numbers"))            ;
  headers << tr                      ("ID"      ) ;
  headers << tr                      ("Prime"   ) ;
  ID       = new QSpinBox            (this      ) ;
  P        = new QSpinBox            (this      ) ;
  Prime    = new QTreeWidget         (this      ) ;
  Prime   -> setColumnCount          (2         ) ;
  Prime   -> setRootIsDecorated      (false     ) ;
  Prime   -> setHeaderLabels         (headers   ) ;
  Prime   -> setAlternatingRowColors (true      ) ;
  Prime   -> show                    (          ) ;
  ID      -> show                    (          ) ;
  P       -> show                    (          ) ;
  ID      -> setValue                (PrimeId   ) ;
  P       -> setMinimum              (1         ) ;
  P       -> setMaximum              (0x7FFFFFFF) ;
  P       -> setValue                (1         ) ;
  ID      -> setSingleStep           (100       ) ;
  P       -> setSingleStep           (1         ) ;
  plan    -> setFont                 (this      ) ;
  connect(ID,SIGNAL(valueChanged(int)),this,SLOT(primeChanged(int)));
  connect(P ,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
  Reload () ;
}

N::PrimeEditor::~PrimeEditor (void)
{
}

void N::PrimeEditor::showEvent(QShowEvent * event)
{
  QWidget::showEvent(event) ;
  if (!isVisible()) return  ;
  Relocation ()             ;
}

void N::PrimeEditor::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event) ;
  Relocation ()               ;
}

bool N::PrimeEditor::FocusIn(void)
{
  return true ;
}

void N::PrimeEditor::Relocation(void)
{
  int w = width  ()        ;
  int h = height ()        ;
  ID    -> move   ( 0,   0);
  ID    -> resize ( w,  28);
  P     -> move   ( 0,  28);
  P     -> resize ( w,  28);
  Prime -> move   ( 0,  56);
  Prime -> resize ( w,h-56);
}

void N::PrimeEditor::primeChanged(int v)
{
  PrimeId = ID->value();
  Reload();
}

void N::PrimeEditor::valueChanged(int v)
{
  SqlConnection SC(plan->sql) ;
  if (SC.open("nPrimeEditor","valueChanged")) {
    QString     Q     ;
    Q = SC.sql.SelectFrom("id,P",plan->Tables[N::Tables::Primes]) +
        " where P >= " + QString::number(P->value()) +
        " order by id asc limit 0,1;";
    if (SC.Fetch(Q)) PrimeId = SC.Uuid(0);
    SC.close();
  };
  SC.remove();
  Reload();
}

void N::PrimeEditor::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::PrimeEditor::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::PrimeEditor::startup(void)
{
  return true ;
}

void N::PrimeEditor::Reload (void)
{
  Prime->clear();
  SqlConnection SC(plan->sql) ;
  if (SC.open("nPrimeEditor","Reload")) {
    QString     Q     ;
    Q = SC.sql.SelectFrom("id,P",plan->Tables[N::Tables::Primes]) +
        " order by id desc limit 0,1;";
    if (SC.Fetch(Q)) {
      ID->setMaximum(SC.Uuid(0));
      ID->setToolTip(tr("Maximum %1").arg(SC.Uuid(0)));
    };
    Q = SC.sql.SelectFrom("id,P",plan->Tables[N::Tables::Primes]) +
        " where id >= " + QString::number(PrimeId) +
        " order by id asc limit 0,100;";
    SqlLoopNow(SC,Q);
      QTreeWidgetItem * item = new QTreeWidgetItem();
      item->setText(0,QString::number(SC.Uuid(0)));
      item->setText(1,QString::number(SC.Uuid(1)));
      Prime->addTopLevelItem(item);
    SqlLoopErr(SC,Q);
    SqlLoopEnd(SC,Q);
    SC.close();
  };
  SC.remove();
}
