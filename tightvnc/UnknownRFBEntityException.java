
class UnknownRFBEntityException extends Exception {
	/**
	 * 
	 */
	private static final long serialVersionUID = -9065382482408523784L;

	public UnknownRFBEntityException(String type, long value) {
		super("Unknown RFB " + type + " " + value);
	}
}
