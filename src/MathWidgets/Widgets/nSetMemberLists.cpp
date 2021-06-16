#include <mathwidgets.h>

N::SetMemberLists:: SetMemberLists (QWidget * parent,Plan * p)
                  : TreeDock       (          parent,       p)
                  , CodeManager    (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SetMemberLists::~SetMemberLists (void)
{
}

void N::SetMemberLists::Configure(void)
{
  PassDragDrop = false                                    ;
  NewTreeWidgetItem            ( head                   ) ;
  head -> setText              ( 0 , tr("Set member")   ) ;
  head -> setText              ( 1 , tr("Identifier")   ) ;
  setWindowTitle               ( tr("Elements")         ) ;
  setDragDropMode              ( DragOnly               ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  setColumnCount               ( 2                      ) ;
  setAllAlignments             ( head , Qt::AlignCenter ) ;
  setFont                      ( head , Fonts::ListView ) ;
  setHeaderItem                ( head                   ) ;
  plan -> setFont              ( this                   ) ;
  MountClicked                 ( 2                      ) ;
}

QMimeData * N::SetMemberLists::dragMime (void)
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
    setMime ( mime , "member/uuid"  , uuid  )    ;
  } else                                         {
    setMime ( mime , "member/uuids" , Uuids )    ;
  }                                              ;
  return mime                                    ;
}

bool N::SetMemberLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::SetMemberLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::SetMemberLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SetMemberLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SetMemberLists::finishDrag(QMouseEvent * event)
{
  return true   ;
}

bool N::SetMemberLists::FocusIn(void)
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

void N::SetMemberLists::setCheckable(QTreeWidgetItem * item)
{
  item -> setCheckState ( 0 , Qt::Unchecked ) ;
}

void N::SetMemberLists::run(int T,ThreadData * d)
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

bool N::SetMemberLists::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::SetMemberLists::List(void)
{
  clear ( )                                     ;
  EnterSQL ( SC , plan->sql )                   ;
    QString Q                                   ;
    Q = QueryMembers (SC)                       ;
    SqlLoopNow ( SC , Q )                       ;
      NewTreeWidgetItem ( IT )                  ;
      SUID    uuid       = SC . Uuid   (0)      ;
      QString identifier = SC . String (1)      ;
      IT -> setData ( 0 , Qt::UserRole , uuid ) ;
      IT -> setText ( 1 , identifier          ) ;
      addTopLevelItem   ( IT )                  ;
    SqlLoopErr ( SC , Q )                       ;
    SqlLoopEnd ( SC , Q )                       ;
    QString name                                ;
    for (int i=0;i<topLevelItemCount();i++)     {
      QTreeWidgetItem * it = topLevelItem(i)    ;
      SUID uuid = nTreeUuid ( it , 0 )          ;
      if (uuid>0)                               {
        name = Name(SC,uuid,plan->LanguageId)   ;
        it -> setText ( 0 , name )              ;
        setCheckable ( it )                     ;
      }                                         ;
    }                                           ;
  LeaveSQL ( SC , plan->sql )                   ;
  Alert    ( Done           )                   ;
  return true                                   ;
}

void N::SetMemberLists::New(void)
{
  removeOldItem     (    )                ;
  NewTreeWidgetItem ( IT )                ;
  addTopLevelItem   ( IT )                ;
  scrollToItem      ( IT )                ;
  QLineEdit * name                        ;
  name = setLineEdit                      (
    IT,0                                  ,
    SIGNAL(editingFinished())             ,
    SLOT  (nameFinished   ())           ) ;
  IT->setData(0,Qt::UserRole  ,0        ) ;
  name->setFocus(Qt::TabFocusReason)      ;
}

void N::SetMemberLists::Delete(void)
{
  QTreeWidgetItem * item = currentItem()    ;
  if (IsNull(item)) return                  ;
  plan->showMessage(tr("Member deletion is not supported now"));
}

void N::SetMemberLists::Paste(void)
{
  QTreeWidgetItem * item = currentItem()    ;
  if (IsNull(item)) return                  ;
  if (currentColumn()<0) return             ;
  QString text = qApp->clipboard()->text()  ;
  if (text.length()<=0) return              ;
  item->setText(currentColumn(),text)       ;
  Ending ( item )                           ;
}

void N::SetMemberLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName     (
    this , tr("Export set members" )                  ,
    plan->User.absolutePath() , "*.txt"             ) ;
  if (filename.length()<=0) return                    ;
  QFile F ( filename )                                ;
  if (!F.open(QIODevice::WriteOnly)) return           ;
  for (int i=0;i<topLevelItemCount();i++)             {
    QTreeWidgetItem * it = topLevelItem(i)            ;
    QString R                                         ;
    R = it->text(1) + "\t" + it->text(0) + "\r\n"     ;
    F . write ( R . toUtf8 ( ) )                      ;
  }                                                   ;
  F . close ( )                                       ;
}

void N::SetMemberLists::Copy(void)
{
  QTreeWidgetItem * item = currentItem()     ;
  if (IsNull(item)) return                   ;
  if (currentColumn()<0) return              ;
  QString text = item->text(currentColumn()) ;
  qApp->clipboard()->setText(text)           ;
}

void N::SetMemberLists::SelectNone(void)
{
  for (int i=0;i<topLevelItemCount();i++)     {
    QTreeWidgetItem * it = topLevelItem(i)    ;
    it -> setCheckState ( 0 , Qt::Unchecked ) ;
  }                                           ;
}

void N::SetMemberLists::SelectAll(void)
{
  for (int i=0;i<topLevelItemCount();i++)   {
    QTreeWidgetItem * it = topLevelItem(i)  ;
    it -> setCheckState ( 0 , Qt::Checked ) ;
  }                                         ;
}

bool N::SetMemberLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                        ;
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())         ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())         ;
  QPoint          global = mapToGlobal(pos)                        ;
  QTreeWidgetItem * item   = itemAt(pos)                           ;
  SUID              member = 0                                     ;
  QAction       * a                                                ;
  if (NotNull(item)) mm.add(101,tr("Sets"))                        ;
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
  mm . setFont(plan)                                               ;
  a = mm.exec(global)                                              ;
  if (IsNull(a)) return true                                       ;
  switch (mm[a])                                                   {
    case 101                                                       :
      member = nTreeUuid ( item , 0 )                              ;
      if (member>0) emit Sets(member)                              ;
    break                                                          ;
    case 102                                                       :
      List ( )                                                     ;
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

void N::SetMemberLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * name = NULL                 ;
  switch (column)                         {
    case 0                                :
    name = setLineEdit                    (
      item,column                         ,
      SIGNAL(editingFinished())           ,
      SLOT  (nameFinished   ())         ) ;
    break                                 ;
    case 1                                :
    name = setLineEdit                    (
      item,column                         ,
      SIGNAL(editingFinished   ())        ,
      SLOT  (identifierFinished())      ) ;
    break                                 ;
  }                                       ;
  if (IsNull(name)) return                ;
  name -> setFocus ( Qt::TabFocusReason ) ;
}

void N::SetMemberLists::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::SetMemberLists::nameFinished(void)
{
  QLineEdit * name = qobject_cast<QLineEdit *>(ItemWidget) ;
  if (NotNull(name))                                       {
    ItemEditing -> setText ( 0 , name->text() )            ;
    Ending (ItemEditing)                                   ;
    removeOldItem()                                        ;
  }                                                        ;
}

void N::SetMemberLists::identifierFinished(void)
{
  QLineEdit * name = qobject_cast<QLineEdit *>(ItemWidget) ;
  if (NotNull(name))                                       {
    ItemEditing -> setText ( 1 , name->text() )            ;
    Ending (ItemEditing)                                   ;
    removeOldItem()                                        ;
  }                                                        ;
}

void N::SetMemberLists::Ending(QTreeWidgetItem * item)
{
  SUID    uu         = nTreeUuid(item,0)             ;
  QString name       = item->text(0)                 ;
  QString identifier = item->text(1)                 ;
  if ( name       . length()<=0 ) return             ;
  if ( identifier . length()<=0 ) return             ;
  for (int i=0;i<identifier.length();i++)            {
    QChar c = identifier.at(i)                       ;
    if (!c.isLetterOrNumber())                       {
      plan->showMessage                              (
        tr("Identifier must be letters or numbers")) ;
      Alert ( Error )                                ;
      return                                         ;
    }                                                ;
  }                                                  ;
  EnterSQL ( SC , plan->sql )                        ;
    uu = assureMember                                (
      SC                                             ,
      uu                                             ,
      vLanguageId                                    ,
      name                                           ,
      identifier                          )          ;
    item -> setData ( 0,Qt::UserRole,uu   )          ;
    setCheckable    ( item                )          ;
  LeaveSQL ( SC , plan->sql )                        ;
  Alert    ( Done           )                        ;
}
