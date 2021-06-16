#include <mathwidgets.h>

N::SetLists:: SetLists    (QWidget * parent,Plan * p)
            : TreeDock    (          parent,       p)
            , CodeManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SetLists::~SetLists (void)
{
}

void N::SetLists::Configure(void)
{
  PassDragDrop = false                                    ;
  NewTreeWidgetItem            ( head                   ) ;
  head -> setText              ( 0 , tr("Name"      )   ) ;
  head -> setText              ( 1 , tr("Type"      )   ) ;
  head -> setText              ( 2 , tr("Uniqueness")   ) ;
  setWindowTitle               ( tr("Sets")             ) ;
  setDragDropMode              ( DragDrop               ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  setColumnCount               ( 3                      ) ;
  setAllAlignments             ( head , Qt::AlignCenter ) ;
  setFont                      ( head , Fonts::ListView ) ;
  setHeaderItem                ( head                   ) ;
  setColumnHidden              ( 1 , true               ) ;
  setColumnHidden              ( 2 , true               ) ;
  plan -> setFont              ( this                   ) ;
  setDropFlag                  ( DropMember , true      ) ;
  MountClicked                 ( 2                      ) ;
}

QMimeData * N::SetLists::dragMime (void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->checkState(0)==Qt::Checked)          {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  if (Uuids.count()<=0)                          {
    QTreeWidgetItem * it = currentItem()         ;
    if (NotNull(it))                             {
      uuid = nTreeUuid(it,0)                     ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "set/uuid"  , uuid  )       ;
  } else                                         {
    setMime ( mime , "set/uuids" , Uuids )       ;
  }                                              ;
  return mime                                    ;
}

bool N::SetLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::SetLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::SetLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SetLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SetLists::finishDrag(QMouseEvent * event)
{
  return true   ;
}

bool N::SetLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  if (source==this) return false ;
  return dropHandler(mime)       ;
}

bool N::SetLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::SetLists::dropMembers(QWidget * source,QPoint pos,const UUIDs & Uuids)
{
  return true ;
}

bool N::SetLists::FocusIn(void)
{
  AssignAction ( Label      , windowTitle ( ) ) ;
  LinkAction   ( Insert     , New         ( ) ) ;
  LinkAction   ( Delete     , Delete      ( ) ) ;
  LinkAction   ( Paste      , Paste       ( ) ) ;
  LinkAction   ( Export     , Export      ( ) ) ;
  LinkAction   ( Copy       , Copy        ( ) ) ;
  LinkAction   ( SelectNone , SelectNone  ( ) ) ;
  LinkAction   ( SelectAll  , SelectAll   ( ) ) ;
  return true                                   ;
}

void N::SetLists::setCheckable(QTreeWidgetItem * item)
{
  item -> setCheckState ( 0 , Qt::Unchecked ) ;
}

void N::SetLists::run(int T,ThreadData * d)
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

bool N::SetLists::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::SetLists::List(void)
{
  NAMEs Tname                                                    ;
  NAMEs Uname                                                    ;
  Tname[N::Sets::Unordered] = tr("Unordered set"        ) ;
  Tname[N::Sets::POSet    ] = tr("Partially ordered set") ;
  Tname[N::Sets::Preorder ] = tr("Preordered set"       ) ;
  Tname[N::Sets::Ordered  ] = tr("Ordered set"          ) ;
  Tname[N::Sets::Totally  ] = tr("Totally ordered set"  ) ;
  Tname[N::Sets::Fuzzy    ] = tr("Fuzzy set"            ) ;
  Uname[0                        ] = tr("No"                   ) ;
  Uname[1                        ] = tr("Yes"                  ) ;
  clear ( )                                                      ;
  EnterSQL ( SC , plan->sql )                                    ;
    QString Q                                                    ;
    Q = QuerySets (SC)                                           ;
    SqlLoopNow ( SC , Q )                                        ;
      NewTreeWidgetItem ( IT )                                   ;
      SUID uuid   = SC . Uuid ( 0 )                              ;
      int  type   = SC . Int  ( 1 )                              ;
      int  unique = SC . Int  ( 2 )                              ;
      if (unique!=1) unique = 0                                  ;
      IT -> setData ( 0 , Qt::UserRole , uuid   )                ;
      IT -> setData ( 1 , Qt::UserRole , type   )                ;
      IT -> setData ( 2 , Qt::UserRole , unique )                ;
      IT -> setText ( 1 , Tname[type  ]         )                ;
      IT -> setText ( 2 , Uname[unique]         )                ;
      addTopLevelItem   ( IT )                                   ;
    SqlLoopErr ( SC , Q )                                        ;
    SqlLoopEnd ( SC , Q )                                        ;
    QString name                                                 ;
    for (int i=0;i<topLevelItemCount();i++)                      {
      QTreeWidgetItem * it = topLevelItem(i)                     ;
      SUID uuid = nTreeUuid ( it , 0 )                           ;
      if (uuid>0)                                                {
        name = Name(SC,uuid,vLanguageId)                         ;
        it -> setText ( 0 , name )                               ;
        setCheckable ( it )                                      ;
      }                                                          ;
    }                                                            ;
  LeaveSQL ( SC , plan->sql )                                    ;
  Alert ( Done )                                                 ;
  return true                                                    ;
}

void N::SetLists::New(void)
{
  removeOldItem     (    )                              ;
  NewTreeWidgetItem ( IT )                              ;
  addTopLevelItem   ( IT )                              ;
  scrollToItem      ( IT )                              ;
  QLineEdit * name                                      ;
  name = setLineEdit                                    (
    IT,0                                                ,
    SIGNAL(editingFinished())                           ,
    SLOT  (nameFinished   ())                         ) ;
  IT->setData(0,Qt::UserRole  , 0                     ) ;
  IT->setData(1,Qt::UserRole  , N::Sets::POSet ) ;
  IT->setData(2,Qt::UserRole  , 1                     ) ;
  IT->setText(1,tr("Partially ordered set")           ) ;
  IT->setText(2,tr("Yes"                  )           ) ;
  name->setFocus(Qt::TabFocusReason)                    ;
}

void N::SetLists::Delete(void)
{
  QTreeWidgetItem * item = currentItem()    ;
  if (IsNull(item)) return                  ;
  plan->showMessage(tr("Set deletion is not supported now"));
}

void N::SetLists::Paste(void)
{
  QTreeWidgetItem * item = currentItem()    ;
  if (IsNull(item)) return                  ;
  if (currentColumn()<0) return             ;
  QString text = qApp->clipboard()->text()  ;
  if (text.length()<=0) return              ;
  item->setText(currentColumn(),text)       ;
  Ending ( item )                           ;
}

void N::SetLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName     (
    this , tr("Export sets" )                         ,
    plan->User.absolutePath() , "*.txt"             ) ;
  if (filename.length()<=0) return                    ;
  QFile F ( filename )                                ;
  if (!F.open(QIODevice::WriteOnly)) return           ;
  for (int i=0;i<topLevelItemCount();i++)             {
    QTreeWidgetItem * it = topLevelItem(i)            ;
    QString R                                         ;
    R = it->text(0) + "\t" + it->text(1)              +
                      "\t" + it->text(2) + "\r\n"     ;
    F . write ( R . toUtf8 ( ) )                      ;
  }                                                   ;
  F . close ( )                                       ;
}

void N::SetLists::Copy(void)
{
  QTreeWidgetItem * item = currentItem()     ;
  if (IsNull(item)) return                   ;
  if (currentColumn()<0) return              ;
  QString text = item->text(currentColumn()) ;
  qApp->clipboard()->setText(text)           ;
}

void N::SetLists::SelectNone(void)
{
  for (int i=0;i<topLevelItemCount();i++)     {
    QTreeWidgetItem * it = topLevelItem(i)    ;
    it -> setCheckState ( 0 , Qt::Unchecked ) ;
  }                                           ;
}

void N::SetLists::SelectAll(void)
{
  for (int i=0;i<topLevelItemCount();i++)   {
    QTreeWidgetItem * it = topLevelItem(i)  ;
    it -> setCheckState ( 0 , Qt::Checked ) ;
  }                                         ;
}

bool N::SetLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                        ;
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())         ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())         ;
  QPoint          global = mapToGlobal(pos)                        ;
  QTreeWidgetItem * item   = itemAt(pos)                           ;
  SUID              member = 0                                     ;
  QAction       * a                                                ;
  if (NotNull(item))                                               {
    mm.add(101,tr("Edit"))                                         ;
    mm.addSeparator()                                              ;
  }                                                                ;
  a = mm.add(111,tr("Type"      )                                ) ;
  a -> setCheckable ( true )                                       ;
  a -> setChecked(!isColumnHidden(1))                              ;
  a = mm.add(112,tr("Uniqueness")                                ) ;
  a -> setCheckable ( true )                                       ;
  a -> setChecked(!isColumnHidden(2))                              ;
  mm.addSeparator()                                                ;
  mm.add(102,tr("Refresh"))                                        ;
  mm.addSeparator()                                                ;
  a = mm.add(201,tr("Enable sorting"))                             ;
  a -> setCheckable ( true )                                       ;
  a -> setChecked(isSortingEnabled())                              ;
  mm.add(202,tr("Ascending sort"    ))                             ;
  mm.add(203,tr("Descending sort"   ))                             ;
  if (NotNull(dock) || NotNull(mdi)) mm.addSeparator()             ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))     ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))     ;
  mm . setFont( plan )                                             ;
  a = mm.exec(global)                                              ;
  if (IsNull(a)) return true                                       ;
  switch (mm[a])                                                   {
    case 102                                                       :
      List ( )                                                     ;
    break                                                          ;
    case 111                                                       :
      setColumnHidden(1,!a->isChecked())                           ;
      resizeColumnToContents(0)                                    ;
      resizeColumnToContents(1)                                    ;
      resizeColumnToContents(2)                                    ;
    break                                                          ;
    case 112                                                       :
      setColumnHidden(2,!a->isChecked())                           ;
      resizeColumnToContents(0)                                    ;
      resizeColumnToContents(1)                                    ;
      resizeColumnToContents(2)                                    ;
    break                                                          ;
    case 201                                                       :
      setSortingEnabled(a->isChecked())                            ;
    break                                                          ;
    case 202                                                       :
      sortItems(currentColumn()<0 ? 0 : currentColumn()            ,
                Qt::AscendingOrder                               ) ;
    break                                                          ;
    case 203                                                       :
      sortItems(currentColumn()<0 ? 0 : currentColumn()            ,
                Qt::DescendingOrder                              ) ;
    break                                                          ;
    case 1000001                                                   :
      emit attachMdi (this,Qt::Vertical)                           ;
    break                                                          ;
    case 1000002                                                   :
      emit attachDock                                              (
        this                                                       ,
        windowTitle()                                              ,
        Qt::RightDockWidgetArea                                    ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea           ) ;
    break                                                          ;
    default                                                        :
    break                                                          ;
  }                                                                ;
  return true                                                      ;
}

void N::SetLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * name   = NULL                                             ;
  QComboBox * Types  = NULL                                             ;
  int         unique = 1                                                ;
  int         type   = N::Sets::POSet                            ;
  NAMEs Tname                                                           ;
  Tname[N::Sets::Unordered] = tr("Unordered set"        )        ;
  Tname[N::Sets::POSet    ] = tr("Partially ordered set")        ;
  Tname[N::Sets::Preorder ] = tr("Preordered set"       )        ;
  Tname[N::Sets::Ordered  ] = tr("Ordered set"          )        ;
  Tname[N::Sets::Totally  ] = tr("Totally ordered set"  )        ;
  Tname[N::Sets::Fuzzy    ] = tr("Fuzzy set"            )        ;
  switch (column)                                                       {
    case 0                                                              :
      name = setLineEdit                                                (
        item,column                                                     ,
        SIGNAL(editingFinished())                                       ,
        SLOT  (nameFinished   ())           )                           ;
      if (IsNull(name)) return                                          ;
      name -> setFocus ( Qt::TabFocusReason )                           ;
    break                                                               ;
    case 1                                                              :
      type = item->data(1,Qt::UserRole).toInt()                         ;
      Types = new QComboBox(this)                                       ;
      N::AddItems(*Types,Tname)                                         ;
      Types->setCurrentIndex(type)                                      ;
      setItemWidget ( item , column , Types )                           ;
      ItemEditing = item                                                ;
      ItemColumn  = column                                              ;
      ItemWidget  = Types                                               ;
      connect(Types,SIGNAL(activated(int)),this,SLOT(TypeChanged(int))) ;
      Types->showPopup()                                                ;
    break                                                               ;
    case 2                                                              :
      unique = item->data(2,Qt::UserRole).toInt()                       ;
      unique = ( unique == 0 ) ? 1 : 0                                  ;
      item->setData(2,Qt::UserRole,unique)                              ;
      item->setText(2,(unique==0) ? tr("No") : tr("Yes") )              ;
      Ending(item)                                                      ;
    break                                                               ;
  }                                                                     ;
}

void N::SetLists::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::SetLists::nameFinished(void)
{
  QLineEdit * name = qobject_cast<QLineEdit *>(ItemWidget) ;
  if (NotNull(name))                                       {
    ItemEditing -> setText ( 0 , name->text() )            ;
    Ending (ItemEditing)                                   ;
    removeOldItem()                                        ;
  }                                                        ;
}

void N::SetLists::TypeChanged(int index)
{
  QComboBox * Types = Casting ( QComboBox,ItemWidget  ) ;
  if (NotNull(Types))                                   {
    ItemEditing -> setText ( 1 , Types->currentText() ) ;
    ItemEditing -> setData ( 1 , Qt::UserRole , index ) ;
    Ending (ItemEditing)                                ;
    removeOldItem()                                     ;
  }                                                     ;
}

void N::SetLists::Ending(QTreeWidgetItem * item)
{
  SUID    uuid       = nTreeUuid(item,0)                  ;
  QString name       = item->text(0)                      ;
  int     type       = item->data(1,Qt::UserRole).toInt() ;
  int     uniqueness = item->data(2,Qt::UserRole).toInt() ;
  if ( name       . length()<=0 ) return                  ;
  EnterSQL ( SC , plan->sql )                             ;
    uuid = assureSet                                      (
      SC                                                  ,
      uuid                                                ,
      plan->LanguageId                                    ,
      name                                                ,
      type                                                ,
      uniqueness                          )               ;
    item -> setData ( 0,Qt::UserRole,uuid )               ;
    setCheckable    ( item                )               ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}
