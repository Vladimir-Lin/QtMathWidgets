#include <mathwidgets.h>

N::DecisionLists:: DecisionLists ( QWidget * parent , Plan * p              )
                 : TreeDock      (           parent ,        p              )
                 , Ownership     ( 0 , Types::None  , Groups::Subordination )
                 , GroupItems    (                           p              )
                 , Sorting       ( Qt::AscendingOrder                       )
                 , dropAction    ( false                                    )
                 , DecisionType  ( Types::DecisionTable                     )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DecisionLists::~DecisionLists(void)
{
}

QSize N::DecisionLists::sizeHint(void) const
{
  return QSize ( 320 , 320 ) ;
}

QMimeData * N::DecisionLists::dragMime (void)
{
  QTreeWidgetItem * IT = currentItem ( )     ;
  if (IsNull(IT)) return NULL                ;
  SUID  uuid  = nTreeUuid(IT,0)              ;
  QMimeData * mime = new QMimeData ( )       ;
  setMime ( mime , "decision/uuid"  , uuid ) ;
  return mime                                ;
}

bool N::DecisionLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::DecisionLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::DecisionLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::DecisionLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::DecisionLists::finishDrag(QMouseEvent * event)
{
  return true ;
}

void N::DecisionLists::Configure(void)
{
  NewTreeWidgetItem            ( head                   ) ;
  /////////////////////////////////////////////////////////
  head -> setText              ( 0 , tr("Decision")     ) ;
  head -> setText              ( 1 , ""                 ) ;
  setWindowTitle               ( tr("Decision lists")   ) ;
  /////////////////////////////////////////////////////////
  setFocusPolicy               ( Qt::WheelFocus         ) ;
  setDragDropMode              ( DragOnly               ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setColumnCount               ( 2                      ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  /////////////////////////////////////////////////////////
  setAllAlignments             ( head , Qt::AlignCenter ) ;
  setFont                      ( head , Fonts::ListView ) ;
  assignHeaderItems            ( head                   ) ;
  /////////////////////////////////////////////////////////
  MountClicked                 ( 2                      ) ;
  plan -> setFont              ( this                   ) ;
  PassDragDrop = false                                    ;
}

bool N::DecisionLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Rename       , Rename      ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Paste        , Paste       ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone  ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll   ( ) ) ;
  LinkAction        ( Language     , Language    ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  if ( ! isStandby  (                              ) ) {
    LinkAction      ( Delete       , Delete      ( ) ) ;
  }                                                    ;
  return true                                          ;
}

void N::DecisionLists::run(int Type,ThreadData * data)
{
  switch ( Type ) {
    case 10001    :
      List ( )    ;
    break         ;
  }               ;
}

bool N::DecisionLists::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::DecisionLists::List(void)
{
  EnterSQL ( SC , plan->sql )                                                ;
    //////////////////////////////////////////////////////////////////////////
    QString name                                                             ;
    SUID    uuid                                                             ;
    UUIDs   Uuids                                                            ;
    //////////////////////////////////////////////////////////////////////////
    if ( isStandby ( ) )                                                     {
      Uuids = SC . Uuids                                                     (
                PlanTable(Decisions)                                         ,
                "uuid"                                                       ,
                QString ( "where type = %1 %2"                               )
                . arg   ( DecisionType                                       )
                . arg   ( SC . OrderBy ( "id" , Sorting ) )                ) ;
    } else                                                                   {
      Uuids = Subordination                                                  (
                SC                                                           ,
                ObjectUuid ( )                                               ,
                ObjectType ( )                                               ,
                DecisionType                                                 ,
                Connexion  ( )                                               ,
                SC . OrderBy ( "position" , Sorting )                      ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    foreach (uuid,Uuids)                                                     {
      NewTreeWidgetItem   ( IT                                             ) ;
      name = SC . getName ( PlanTable(Names) , "uuid" , vLanguageId , uuid ) ;
      IT -> setData       ( 0 , Qt::UserRole , uuid                        ) ;
      IT -> setText       ( 0 , name                                       ) ;
      addTopLevelItem     ( IT                                             ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
  LeaveSQL     ( SC , plan->sql )                                            ;
  emit AutoFit (                )                                            ;
  Alert        ( Done           )                                            ;
}

void N::DecisionLists::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

void N::DecisionLists::Insert(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::DecisionLists::Rename(void)
{
  QTreeWidgetItem * IT        ;
  IT = currentItem ( )        ;
  if ( IsNull ( IT ) ) return ;
  doubleClicked ( IT , 0 )    ;
}

void N::DecisionLists::doubleClicked(QTreeWidgetItem *item, int column)
{
  QLineEdit * line                         ;
  //////////////////////////////////////////
  removeOldItem ( )                        ;
  //////////////////////////////////////////
  switch ( column )                        {
    case 0                                 : // Name
      line = setLineEdit                   (
               item                        ,
               column                      ,
               SIGNAL(editingFinished())   ,
               SLOT  (NameFinished   ()) ) ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
  }                                        ;
}

SUID N::DecisionLists::assureItem (
        SqlConnection & SC        ,
        SUID            u         ,
        QString         name      )
{
  if ( u <= 0 )                                                   {
    u  = SC . Unique ( PlanTable(MajorUuid) , "uuid" , 357912   ) ;
    SC . assureUuid  ( PlanTable(MajorUuid) , u , DecisionType  ) ;
    SC . assureUuid  ( PlanTable(Decisions) , u , DecisionType  ) ;
    if ( ( u > 0 ) && ( ! isStandby ( ) ) )                       {
      GroupItems::Join                                            (
        SC                                                        ,
        ObjectUuid ( )                                            ,
        ObjectType ( )                                            ,
        DecisionType                                              ,
        Connexion  ( )                                            ,
        0                                                         ,
        u                                                       ) ;
    }                                                             ;
  }                                                               ;
  if ( u > 0 )                                                    {
    SC . assureName ( PlanTable(Names) , u , vLanguageId , name ) ;
  }                                                               ;
  return u                                                        ;
}

void N::DecisionLists::NameFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing , ItemColumn ) ;
  QLineEdit * name = Casting   ( QLineEdit   , ItemWidget ) ;
  ///////////////////////////////////////////////////////////
  if (IsNull(name))                                         {
    removeOldItem ( )                                       ;
    return                                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  QString decision = name->text()                           ;
  if (decision.length()<=0)                                 {
    removeOldItem ( )                                       ;
    return                                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  EnterSQL                   ( SC , plan->sql             ) ;
    uuid = assureItem        ( SC , uuid , decision       ) ;
    if ( uuid > 0 )                                         {
      ItemEditing -> setData ( 0 , Qt::UserRole , uuid    ) ;
      ItemEditing -> setText ( 0 , decision               ) ;
      emit Update            ( this , uuid                ) ;
    }                                                       ;
  ErrorSQL                   ( SC , plan->sql             ) ;
  LeaveSQL                   ( SC , plan->sql             ) ;
  ///////////////////////////////////////////////////////////
  removeOldItem              (                            ) ;
  Alert                      ( Done                       ) ;
}

void N::DecisionLists::Delete(void)
{
  if ( isStandby ( ) ) return ;
}

void N::DecisionLists::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::DecisionLists::Paste(QString text)
{
  QStringList L = text . split ( '\n' )               ;
  if ( L . count ( ) <= 0 ) return                    ;
  EnterSQL                  ( SC , plan->sql        ) ;
    QString n                                         ;
    SUID    u                                         ;
    foreach                 ( n , L                 ) {
      NewTreeWidgetItem     ( it                    ) ;
      u   = assureItem      ( SC , 0            , n ) ;
      it -> setData         ( 0  , Qt::UserRole , u ) ;
      it -> setText         ( 0  , n                ) ;
      addTopLevelItem       ( it                    ) ;
      plan -> processEvents (                       ) ;
    }                                                 ;
  ErrorSQL                  ( SC , plan->sql        ) ;
  LeaveSQL                  ( SC , plan->sql        ) ;
  Alert                     ( Done                  ) ;
}

void N::DecisionLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Export decision lists") ,
                       plan->Temporary("")         ,
                       "*.*\n"
                       "*.txt"                   ) ;
  nDropOut ( filename.length() <= 0 )              ;
  //////////////////////////////////////////////////
  QString T = toText ( )                           ;
  if ( T . length ( ) <= 0 ) return                ;
  File::toFile ( filename , T )                    ;
}

bool N::DecisionLists::Menu(QPoint)
{
  nScopedMenu ( mm , this )                         ;
  QTreeWidgetItem * IT = currentItem ( )            ;
  QAction         * aa                              ;
  if ( NotNull(IT) )                                {
    mm . add ( 101 , tr("Edit")    )                ;
  }                                                 ;
  mm   . add ( 102 , tr("Refresh") )                ;
  ///////////////////////////////////////////////////
  DockingMenu     ( mm        )                     ;
  mm . setFont    ( plan      )                     ;
  aa = mm . exec  (           )                     ;
  if ( IsNull     ( aa      ) ) return true         ;
  if ( RunDocking ( mm , aa ) ) return true         ;
  ///////////////////////////////////////////////////
  switch (mm[aa])                                   {
    case 101                                        :
      if (NotNull(IT))                              {
        SUID u = nTreeUuid ( IT , 0 )               ;
        if ( u > 0 )                                {
          emit Edit ( this , IT -> text ( 0 ) , u ) ;
        }                                           ;
      }                                             ;
    break                                           ;
    case 102                                        :
      List ( )                                      ;
    break                                           ;
    default                                         :
    break                                           ;
  }                                                 ;
  return true                                       ;
}
